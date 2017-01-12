package org.alcatelLucent.xuggle;

import org.alcatelLucent.h263.depacketizer.H263Depacketizer;
import org.apache.mina.core.buffer.IoBuffer;
import org.red5.logging.Red5LoggerFactory;
import org.red5.server.webapp.sip.HeaderH264;
import org.red5.server.webapp.sip.RTMPStreamListener.MyVideoPacket;
import org.red5.server.webapp.sip.RTMPStreamSender;
import org.red5.server.webapp.sip.Util;
import org.slf4j.Logger;

import com.xuggle.ferry.IBuffer;
import com.xuggle.xuggler.*;
import com.xuggle.xuggler.ICodec.ID;

/**
 * This class implements a H263+-to-H264 transcoder.
 * <p>
 * Transcoding is performed in a new thread using Xuggle.
 * </p>
 */
public class H263toH264 extends Thread {

	protected static Logger log = Red5LoggerFactory.getLogger(H263toH264.class,
			"sip");

	// Initial settings specific to desired output format
	static String containerFormatShortName = "flv";
	static ID codecID = ICodec.ID.CODEC_ID_FLV1;
	static IPixelFormat.Type supportedOutputPixelType = IPixelFormat.Type.YUV420P;

	final static String ffpreset = "webapps/sip/WEB-INF/ffpresets/libx264-myHigh.ffpreset";
	// final static String ffpreset
	// ="webapps/sip/WEB-INF/ffpresets/libx264-mybaselineSIPCom.ffpreset";

	private IStreamCoder decoder;
	// private IVideoResampler inputResampler;
	private int index = 0;

	private volatile boolean running = false;
	private boolean modify = false;
	private boolean modified = true;
	private boolean logModify = false;
	RTMPStreamSender rtmpStreamSender = null;
	Thread myThread = null;

	private H263Depacketizer depacketizer = null;

	boolean started = false;

	int outputWidth = -1;
	int outputHeight = -1;

	// public static final int VIDEO_WIDTH = 176;
	// public static final int VIDEO_HEIGHT = 144;
	private final static int KEYRATE_H264 = 15;
	public static final int VIDEO_WIDTH = 352;
	public static final int VIDEO_HEIGHT = 288;
	// public static final int VIDEO_WIDTH = 528;
	// public static final int VIDEO_HEIGHT = 432;
	// private static final int VIDEO_FPS = 15;

	IPacket encodedPacket;

	MyProtocolHandler myHandler = null;

	private IContainer outputContainer;

	private long firstRTPTimestamp = 0;
	private long currentRTPTimestamp = 0;

	long computationTime = 0;
	long delta = 0;

	public H263toH264(RTMPStreamSender rtmpStreamSender,
			final H263Depacketizer depacketizer) {
		running = true;
		this.rtmpStreamSender = rtmpStreamSender;
		this.depacketizer = depacketizer;
		setupInputDecoder();
		setupOutputContainer(VIDEO_WIDTH, VIDEO_HEIGHT);
	}

	public H263toH264() {
		setupOutputContainer(VIDEO_WIDTH, VIDEO_HEIGHT);
	}

	public void setVideoOutputSize(final int outputWidth, final int outputHeight) {
		this.outputWidth = outputWidth;
		this.outputHeight = outputHeight;
		modify = true;
		logModify = true;
	}

	@Override
	public void run() {

		index = 0;
		IVideoPicture decodedPicture = null;
		IStreamCoder encoder = getEncoder();

		// Create a video picture to serve as a destination for the
		// resampler
		IVideoPicture resampledPicture = IVideoPicture
				.make(encoder.getPixelType(), encoder.getWidth(),
						encoder.getHeight());

		long ts = 0;
		int counter = 0;
		// Transcode input to output
		while (running) {
			// Read picture from input
			try {
				decodedPicture = readNextYUVVideoPicture();
			} catch (InterruptedException ie) {
				log.warn(ie.getMessage());
			}
			if (decodedPicture != null) {
				if (index < 50) {
					log.info("Successfully got a DECODED video picture Size (WxH) "
							+ decodedPicture.getWidth()
							+ "x"
							+ decodedPicture.getHeight()
							+ " Key Frame "
							+ decodedPicture.isKeyFrame());
					// log.debug("Raw packet -> ts = "
					// + decodedPicture.getTimeStamp());
				}
				if (modify) {
					if (logModify) {
						log.info("Try to modify dynamically the video size....");
						logModify = false;
					}
					log.debug("Got new key frame, modifying video size");
					closeCurrentOutputContainer();
					setupOutputContainer(outputWidth, outputHeight);
					encoder = getEncoder();
					resampledPicture = IVideoPicture.make(
							encoder.getPixelType(), encoder.getWidth(),
							encoder.getHeight());
					IoBuffer avc = getAvcConfiguration().getHeaderH264();
					rtmpStreamSender.pushAVCConfiguration(avc);
					modify = false;
					modified = false;
					log.info(" Video size: modified -> " + outputWidth + "x"
							+ outputHeight);
				}
				if (decodedPicture.getWidth() != encoder.getWidth()
						&& decodedPicture.getHeight() != encoder.getHeight()) {
					IVideoResampler resampler = getResampler(
							decodedPicture.getWidth(),
							decodedPicture.getHeight(), encoder.getWidth(),
							encoder.getHeight());
					resampler.resample(resampledPicture, decodedPicture);
					if (index < 15) {
						log

						.info("Successfully got a RESAMPLED picture Size (WxH) "
								+ resampledPicture.getWidth()
								+ "x"
								+ resampledPicture.getHeight());
					}
				} else {
					resampledPicture = decodedPicture;
				}

				// Encode resampled picture to a packet
				int encodeVideoRetCode = encoder.encodeVideo(encodedPacket,
						resampledPicture, -1);
				if (encodeVideoRetCode < 0) {
					IError err = IError.make(encodeVideoRetCode);
					log.error("IStreamCoder.encodeVideo() returned an error: "
							+ err.getType() + ", " + err.getDescription());
				}

				// Write encoded packet to output inputContainer
				if (encodedPacket.isComplete()) {
					if (index < 15) {
						log.info("Successfully got a RE-ENCODED picture-> ts = "
								+ encodedPacket.getTimeStamp()
								+ " Size (WxH) = "
								+ encoder.getWidth()
								+ "x"
								+ encoder.getHeight());
					}
					if (encodedPacket.isKeyPacket()) {
						this.modified = true;
					}
					int writePacketRetCode = outputContainer
							.writePacket(encodedPacket);

					if (writePacketRetCode != 0) {
						IError err = IError.make(writePacketRetCode);
						log.error("IContainer.writePacket() returned an error: "
								+ err.getType() + ", " + err.getDescription());
					}

					int flushCode = outputContainer.flushPackets();
					if (flushCode < 0) {
						log.warn("Flush into output container:  NOK!!!");
					}
					if (modified) {

						ts = currentRTPTimestamp;

						byte[] data = myHandler.getLastPacket();
						byte[] frame = new byte[data.length - 11 - 4];

						System.arraycopy(data, 11, frame, 0, frame.length);

						if (index < 15) {
							Util.display20Hexa(frame, "H264 frame", (int) ts,
									H263toH264.class, true);
						}
						// finally push timestamp and data to rtmpStreamSender
						rtmpStreamSender.pushSorensonVideoData(ts, frame);
					}
				} else {
					log.warn("Incomplete RE-ENCODED H264 packet -> dropped");
				}
			} else {
				log.warn("Could not read H263+ picture -> dropped");
			}
			if ((System.nanoTime() - computationTime) / 1000000 > 50) {
				Float ratio = new Float(100 * (float) counter / (index + 1));
				log.debug("Computation time(n°" + index + ") = "
						+ (System.nanoTime() - computationTime) / 1000000
						+ " ms ratio " + ratio.toString() + " %");
				counter++;
			}
			index++;
		}

		int flushEncoderRetCode;
		do {
			flushEncoderRetCode = encoder.encodeVideo(encodedPacket, null, -1);
			log.info("Flushed a packet through the video encoder, which returned "
					+ flushEncoderRetCode);
			if (encodedPacket.isComplete()) {
				log.info("The flushed packet is complete");
			} else {
				log.info("The flushed packet is incomplete");
			}
		} while (flushEncoderRetCode >= 0 && encodedPacket.isComplete());

		int cwtRetCode = outputContainer.writeTrailer();
		if (cwtRetCode != 0) {
			IError err = IError.make(cwtRetCode);
			log.error("IContainer.writeHeader() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return;
		}

		encoder.close();
		encoder = null;

		int closeRetcode = outputContainer.close();
		if (closeRetcode >= 0) {
			log.info("Video outputContainer: Closed");
		} else {
			log.warn("Video outputContainer: NOT Closed !!!!!");
		}
		outputContainer = null;

		myThread = null;

		log.info("Video (Xuggle) transcoding: stopped");

	}

	public HeaderH264 getAvcConfiguration() {

		String sdp = outputContainer.createSDPData();
		System.out.println("sdp:\n" + sdp);
		String name = "sprop-parameter-sets";
		int lenght = name.length();
		int startIndex = sdp.indexOf("sprop-parameter-sets");
		String SPSPPS = sdp.substring(startIndex + lenght + 1);
		if (SPSPPS.indexOf("\n") != -1) {
			SPSPPS = SPSPPS.substring(0, SPSPPS.indexOf("\n"));
		}
		System.out.println("SPSPPS = " + SPSPPS);
		String SPS = SPSPPS.substring(0, SPSPPS.indexOf(","));
		System.out.println("PPS = " + SPS);
		String PPS = SPSPPS.substring(SPSPPS.indexOf(",") + 1, SPSPPS.length());
		System.out.println("SPS = " + PPS);

		Base64 base64 = new Base64();
		byte[] sps = base64.decode(SPS);
		byte[] pps = base64.decode(PPS);

		Util.display20Hexa(sps, "sps", 0, H263toH264.class, true);
		Util.display20Hexa(pps, "pps", 0, H263toH264.class, true);

		HeaderH264 header = new HeaderH264();

		header.setSequenceParameterSet(sps);
		header.setPictureParameterSet(pps);

		byte[] avcConfspspps = header.getHeaderH264().array();

		Util.displayByte(avcConfspspps, "avcConf from sps pps",
				H263toH264.class);
		return header;
	}

	public void halt() {
		running = false;
		if (myThread != null) {
			log.debug("Interruption -> name " + myThread.getName() + " Id "
					+ myThread.getId());
			myThread.interrupt();
		}
	}

	protected boolean closeCurrentOutputContainer() {
		IStreamCoder encoder = getEncoder();
		int flushEncoderRetCode;
		do {
			flushEncoderRetCode = encoder.encodeVideo(encodedPacket, null, -1);
			log.info("Flushed a packet through the video encoder, which returned "
					+ flushEncoderRetCode);
			if (encodedPacket.isComplete()) {
				log.info("The flushed packet is complete");
			} else {
				log.info("The flushed packet is incomplete");
			}
		} while (flushEncoderRetCode >= 0 && encodedPacket.isComplete());

		int cwtRetCode = outputContainer.writeTrailer();
		if (cwtRetCode != 0) {
			IError err = IError.make(cwtRetCode);
			log.error("IContainer.writeHeader() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}

		encoder.close();
		encoder = null;

		int closeRetcode = outputContainer.close();
		if (closeRetcode >= 0) {
			log.info("Video outputContainer: Closed");
		} else {
			log.warn("Video outputContainer: NOT Closed !!!!!");
		}
		outputContainer = null;
		return true;
	}

	private boolean setupOutputContainer(int width, int height) {

		// Create and configure the container format object
		IContainerFormat encodedContainerFormat = IContainerFormat.make();
		int cfRetCode = encodedContainerFormat.setOutputFormat(
				containerFormatShortName, null, null);

		if (cfRetCode != 0) {
			IError err = IError.make(cfRetCode);
			log.error("IContainerFormat.setOutputFormat() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}
		log.info("Successfully set output format on IContainerFormat = "
				+ encodedContainerFormat.getOutputFormatLongName());

		// Create and open the output container
		outputContainer = IContainer.make();

		myHandler = new MyProtocolHandler();
		int cRetCode = outputContainer.open(myHandler, IContainer.Type.WRITE,
				encodedContainerFormat, true, false);

		if (cRetCode != 0) {
			IError err = IError.make(cRetCode);
			log.error("IContainer.open() returned an error: " + err.getType()
					+ ", " + err.getDescription());
			return false;
		}
		log.info("Successfully opened output IContainer");
		outputContainer.addNewStream(0);

		// Create a packet to serve as a destination for the encoder's output
		encodedPacket = IPacket.make();

		// Create and configure the encoder

		IStreamCoder encoder = getEncoder(outputContainer);
		boolean ok = setEncoder(encoder, width, height);
		if (!ok) {
			log.error("Set encoder return NOK !!!");
			return false;
		}

		int cwhRetCode = outputContainer.writeHeader();

		if (cwhRetCode != 0) {
			IError err = IError.make(cwhRetCode);
			log.error("IContainer.writeHeader() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}

		log.info("Successfully wrote header to IContainer");
		return true;
	}

	private IStreamCoder getEncoder() {
		IStreamCoder encoder = null;
		if (outputContainer != null && outputContainer.getStream(0) != null) {
			encoder = outputContainer.getStream(0).getStreamCoder();
		}
		return encoder;
	}

	private IStreamCoder getEncoder(IContainer outputContainer) {
		IStreamCoder encoder = null;
		if (outputContainer != null && outputContainer.getStream(0) != null) {
			encoder = outputContainer.getStream(0).getStreamCoder();
		}
		return encoder;
	}

	private boolean setEncoder(IStreamCoder encoder, int width, int height) {
		if (encoder == null) {
			return false;
		}
		log.info("Setting up the output stream coder for video...");
		ICodec.ID videoCodecH264 = ICodec.ID.CODEC_ID_H264;
		encoder.setCodec(videoCodecH264);
		int configuring = Configuration.configure(ffpreset, encoder);
		if (configuring != 0) {
			IError err = IError.make(configuring);
			log.error("Configuration.configure(...) returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}
		// encoder = AVCConfiguration.setH264encoderProperties(encoder);

		encoder.setNumPicturesInGroupOfPictures(KEYRATE_H264);
		encoder.setTimeBase(IRational.make(1, 1000));
		encoder.setPixelType(supportedOutputPixelType);
		encoder.setWidth(width);
		encoder.setHeight(height);
		// encoder.setAutomaticallyStampPacketsForStream(true);
		int encOpenRetVal = encoder.open();
		if (encOpenRetVal != 0) {
			IError err = IError.make(encOpenRetVal);
			log.error("IStreamCoder.open() returned an error: " + err.getType()
					+ ", " + err.getDescription());
			return false;
		}
		return true;
	}

	private IVideoPicture readNextYUVVideoPicture() throws InterruptedException {
		/*
		 * Now, we start walking through the container looking at each packet.
		 */
		MyVideoPacket packet = null;
		IVideoPicture newPic = null;
		if (myThread == null) {
			myThread = Thread.currentThread();
		}
		long to = System.nanoTime();
		packet = depacketizer.OUTPUT_QUEUE.take();

		// packet = H263Depacketizer.OUTPUT_QUEUE.take();
		if (index < 15) {
			H263toH264.log.debug("packet #" + index + " isKey "
					+ packet.isKey() + " ts = " + packet.getTimeStamp());
			Util.display20Hexa(packet.getData(), "H263+ frame",
					(int) packet.getTimeStamp(), H263toH264.class, true);
			// log.debug("H263+ packet length: " + packet.getData().length);
		}
		long currentDelta = System.nanoTime() - to;
		if (delta != 0) {
			float r0 = (float) (index - 1) / (float) index;
			float r1 = (float) 1 / (float) index;
			delta = (long) (r0 * delta + r1 * currentDelta);
		} else {
			delta = currentDelta;
		}

		computationTime = System.nanoTime();

		if (index == 0) {
			firstRTPTimestamp = packet.getTimeStamp();
		}

		// log.debug("Queue Size " + H263Depacketizer.OUTPUT_QUEUE.size());

		/*
		 * We allocate a new picture to get the data out of Xuggler
		 */

		IVideoPicture picture = IVideoPicture.make(decoder.getPixelType(),
				decoder.getWidth(), decoder.getHeight());
		IBuffer iBuffer = IBuffer.make(null, packet.getData(), 0,
				packet.getData().length);
		IPacket iPacket = IPacket.make(iBuffer);
		int pksz = iPacket.getSize();
		iPacket.setComplete(true, pksz);
		iPacket.setTimeStamp(packet.getTimeStamp() - firstRTPTimestamp);
		// iPacket.setKeyPacket(packet.isKey());
		iPacket.setKeyPacket(true);
		if (index < 15) {
			H263toH264.log.debug("ipacket #" + index + " isKey "
					+ iPacket.isKey() + " ts = " + iPacket.getTimeStamp());
		}
		currentRTPTimestamp = iPacket.getTimeStamp() / 90;

		int offset = 0;
		while (offset < iPacket.getSize()) {
			// Now, we decode the video, checking for any errors.
			int bytesDecoded = decoder.decodeVideo(picture, iPacket, offset);
			if (bytesDecoded < 0) {
				return null;
			}
			offset += bytesDecoded;

			/*
			 * Some decoders will consume data in a packet, but will not be able
			 * to construct a full video picture yet. Therefore you should
			 * always check if you got a complete picture from the decoder
			 */
			if (picture.isComplete()) {
				newPic = picture;
			} else if (offset == iPacket.getSize()) {
				H263toH264.log
						.warn("Picture seems to be completly decoded BUT isComplete = false");
				newPic = picture;
			}

		}
		// newPic.setTimeStamp(iPacket.getTimeStamp());
		// log.debug("Raw frame length: " + newPic.getData().getSize());
		return newPic;
	}

	private void setupInputDecoder() {
		decoder = IStreamCoder.make(IStreamCoder.Direction.DECODING);
		decoder.setCodec(ICodec.ID.CODEC_ID_H263);
		decoder.setPixelType(IPixelFormat.Type.YUV420P);
		decoder.setBitRate(200000);
		decoder.setTimeBase(IRational.make(1, 90000));
		openInputVideoCoder();
	}

	private void openInputVideoCoder() {
		if (decoder.open() < 0) {
			throw new RuntimeException(
					"could not open video decoder for container");
		}
	}

	private IVideoResampler getResampler(int inputW, int inputH, int outputW,
			int outputH) {
		IVideoResampler resampler = null;
		resampler = IVideoResampler.make(outputW, outputH,
				IPixelFormat.Type.YUV420P, inputW, inputH,
				IPixelFormat.Type.YUV420P);

		if (resampler == null) {
			throw new RuntimeException("could not create color space resampler");
		}
		return resampler;
	}

	private void logProperties() {
		String prop = null;
		IProperty metaProp = null;
		for (int i = 0; i < decoder.getNumProperties(); i++) {
			metaProp = decoder.getPropertyMetaData(i);
			prop = metaProp.getName();
			log.debug("Property #" + i + ": " + prop + " = "
					+ decoder.getPropertyAsString(prop) + " --- (default = "
					+ metaProp.getDefault() + ")");
		}
		log.debug("Auto stamps packets: "
				+ decoder.getAutomaticallyStampPacketsForStream());
		log.debug("Bit rate: " + decoder.getBitRate());
		log.debug("Bit rate tolerance: " + decoder.getBitRateTolerance());
		log.debug("Channels: " + decoder.getChannels());
		log.debug("Codec: " + decoder.getCodec());
		log.debug("Codec ID: " + decoder.getCodecID());
		log.debug("Codec tag: " + decoder.getCodecTag());
		log.debug("Codec type: " + decoder.getCodecType());
		log.debug("Dimensions: " + decoder.getWidth() + "x"
				+ decoder.getHeight());
		log.debug("Direction: " + decoder.getDirection());
		log.debug("Global quality: " + decoder.getGlobalQuality());
		log.debug("Pixel type: " + decoder.getPixelType());
		log.debug("Sample rate: " + decoder.getSampleRate());
		log.debug("Sample format: " + decoder.getSampleFormat());
		log.debug("Time base: " + decoder.getTimeBase());
	}
	// private IStreamCoder getEncoder(int width, int height) {
	//
	// IStreamCoder encoder = outputContainer.getStream(0).getStreamCoder();
	// //encoder.setCodec(ICodec.ID.CODEC_ID_FLV1);
	//
	// log.info("Setting up the output stream coder for video...");
	// ICodec.ID videoCodecH264 = ICodec.ID.CODEC_ID_H264;
	// encoder.setCodec(videoCodecH264);
	// int configuring = Configuration.configure(ffpreset, encoder);
	//
	// encoder.setNumPicturesInGroupOfPictures(KEYRATE_H264);
	// encoder.setTimeBase(IRational.make(1, 1000));
	// encoder.setPixelType(supportedOutputPixelType);
	// encoder.setWidth(width);
	// encoder.setHeight(height);
	// encoder.setChannels(0);
	// encoder.setGlobalQuality(0);
	// // encoder.setAutomaticallyStampPacketsForStream(true);
	// int encOpenRetVal = encoder.open();
	// if (encOpenRetVal != 0) {
	// IError err = IError.make(encOpenRetVal);
	// log.error("IStreamCoder.open() returned an error: " + err.getType()
	// + ", " + err.getDescription());
	// return null;
	// }
	// return encoder;
	// }

}
