package org.alcatelLucent.xuggle;

import org.alcatelLucent.h263.depacketizer.H263Depacketizer;
import org.red5.logging.Red5LoggerFactory;
import org.red5.server.webapp.sip.RTMPStreamListener.MyVideoPacket;
import org.red5.server.webapp.sip.RTMPStreamSender;
import org.red5.server.webapp.sip.Util;
import org.red5.server.webapp.sip.VideoTimer;
import org.slf4j.Logger;

import com.xuggle.ferry.IBuffer;
import com.xuggle.xuggler.*;
import com.xuggle.xuggler.ICodec.ID;

/**
 * This class implements an H263+-to-Sorenson transcoder.
 * <p>
 * Transcoding is performed in a new thread using Xuggle.
 * </p>
 * 
 * @author Nicolas Canceill
 */
public class H263toSorenson extends Thread {

	protected static Logger log = Red5LoggerFactory.getLogger(
			H263toSorenson.class, "sip");
	private final static int LOG_INDEX_MAX = 30;

	// Initial settings specific to desired output format
	static String containerFormatShortName = "flv";
	static ID codecID = ICodec.ID.CODEC_ID_FLV1;
	static IPixelFormat.Type supportedOutputPixelType = IPixelFormat.Type.YUV420P;

	private IStreamCoder decoder;
	private IVideoResampler inputResampler;
	private int index = 0;

	private boolean running = false;
	private boolean modify = false;
	RTMPStreamSender rtmpStreamSender = null;
	Thread myThread = null;

	boolean started = false;

	int outputWidth = -1;
	int outputHeight = -1;

	// public static final int VIDEO_WIDTH = 176;
	// public static final int VIDEO_HEIGHT = 144;
	private final static int KEYRATE_SORENSON = 15;
	public static final int VIDEO_WIDTH = 352;
	public static final int VIDEO_HEIGHT = 288;
	// private static final int VIDEO_FPS = 15;

	IPacket encodedPacket;

	MyProtocolHandler myHandler = null;

	private IContainer outputContainer;

	private long firstRTPTimestamp = 0;
	private long currentRTPTimestamp = 0;

	long computationTime = 0;
	long delta = 0;

	protected H263Depacketizer depacketizer;

	public H263toSorenson(final RTMPStreamSender rtmpStreamSender,
			final H263Depacketizer depacketizer) {
		running = true;
		this.depacketizer = depacketizer;
		this.rtmpStreamSender = rtmpStreamSender;
	}

	public void setVideoOutputSize(final int outputWidth, final int outputHeight) {
		this.outputWidth = outputWidth;
		this.outputHeight = outputHeight;
		modify = true;
	}

	@Override
	public void run() {

		index = 0;

		setupInputDecoder();

		// Create and configure the container format object
		final IContainerFormat encodedContainerFormat = IContainerFormat.make();
		final int cfRetCode = encodedContainerFormat.setOutputFormat(
				H263toSorenson.containerFormatShortName, null, null);
		if (cfRetCode != 0) {
			final IError err = IError.make(cfRetCode);
			System.out
					.println("IContainerFormat.setOutputFormat() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return;
		}
		H263toSorenson.log
				.info("Successfully set output format on IContainerFormat = "
						+ encodedContainerFormat.getOutputFormatLongName());

		// Create and open the output container
		outputContainer = IContainer.make();

		myHandler = new MyProtocolHandler();
		final int cRetCode = outputContainer.open(myHandler,
				IContainer.Type.WRITE, encodedContainerFormat, true, false);

		if (cRetCode != 0) {
			final IError err = IError.make(cRetCode);
			H263toSorenson.log.error("IContainer.open() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return;
		}
		H263toSorenson.log.info("Successfully opened output IContainer");
		outputContainer.addNewStream(0);

		// Create a packet to serve as a destination for the encoder's output

		encodedPacket = IPacket.make();

		// Create and configure the encoder

		IStreamCoder encoder = getEncoder(H263toSorenson.VIDEO_WIDTH,
				H263toSorenson.VIDEO_HEIGHT);
		if (encoder == null) {
			return;
		}

		int cwhRetCode = outputContainer.writeHeader();

		if (cwhRetCode != 0) {
			final IError err = IError.make(cwhRetCode);
			H263toSorenson.log
					.error("IContainer.writeHeader() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return;
		}

		H263toSorenson.log.info("Successfully wrote header to IContainer");

		IVideoPicture originalInputPic = null;

		// Create a video picture to serve as a destination for the
		// inputResampler's output
		IVideoPicture resampledOutputPic = IVideoPicture
				.make(encoder.getPixelType(), encoder.getWidth(),
						encoder.getHeight());

		long previousTimestamp = 0;
		final long currentTimestamp = 0;
		// long delta = 0;
		// long currentDelta = -1;
		long ts = 0;

		int counter = 0;

		// Transcode input to output
		while (running) {
			// Read picture from input
			try {
				originalInputPic = readNextYUVVideoPicture();
			} catch (final InterruptedException ie) {
				H263toSorenson.log.warn(ie.getMessage());
			}
			if (originalInputPic != null && index >= 0) {
				if (index < H263toSorenson.LOG_INDEX_MAX) {
					H263toSorenson.log
							.debug("Successfully got a video picture from the input source Size (WxH) "
									+ originalInputPic.getWidth()
									+ "x"
									+ originalInputPic.getHeight());
					H263toSorenson.log.debug("Raw packet -> ts = "
							+ originalInputPic.getTimeStamp());
				}
				if (modify && originalInputPic.isKeyFrame()) {
					H263toSorenson.log
							.info("Try to modify dynamicaly the video size....");
					setInputResampler(originalInputPic.getWidth(),
							originalInputPic.getHeight(), outputWidth,
							outputHeight);
					resampledOutputPic = IVideoPicture.make(
							inputResampler.getOutputPixelFormat(),
							inputResampler.getOutputWidth(),
							inputResampler.getOutputHeight());
					closeCurrentOutputContainer(encoder);
					final boolean ok = setOutputContainer();
					if (!ok) {
						H263toSorenson.log
								.error("Could not set the new outputContainer !!!");
					}
					encoder = getEncoder(outputWidth, outputHeight);
					cwhRetCode = outputContainer.writeHeader();

					if (cwhRetCode != 0) {
						final IError err = IError.make(cwhRetCode);
						H263toSorenson.log
								.error("IContainer.writeHeader() returned an error: "
										+ err.getType()
										+ ", "
										+ err.getDescription());
						return;
					}

					H263toSorenson.log
							.info("Successfully wrote the new header to IContainer");
					modify = false;
					H263toSorenson.log.info(" Video size: modified -> "
							+ outputWidth + "x" + outputHeight);
				}
				setInputResampler(originalInputPic.getWidth(),
						originalInputPic.getHeight(), encoder.getWidth(),
						encoder.getHeight());
				inputResampler.resample(resampledOutputPic, originalInputPic);
				if (index < H263toSorenson.LOG_INDEX_MAX) {
					H263toSorenson.log
							.debug("Successfully got a resampled picture from the input source Size (WxH) "
									+ resampledOutputPic.getWidth()
									+ "x"
									+ resampledOutputPic.getHeight());
				}
				// Encode resampled picture to a packet
				final int encodeVideoRetCode = encoder.encodeVideo(
						encodedPacket, resampledOutputPic, -1);
				if (encodeVideoRetCode < 0) {
					final IError err = IError.make(encodeVideoRetCode);
					H263toSorenson.log
							.error("IStreamCoder.encodeVideo() returned an error: "
									+ err.getType()
									+ ", "
									+ err.getDescription());
				}

				// Write encoded packet to output inputContainer
				if (encodedPacket.isComplete()) {
					VideoTimer.setFrameEncodingTime(System.nanoTime() / 1000);
					if (index < H263toSorenson.LOG_INDEX_MAX) {
						H263toSorenson.log
								.debug("Successfully got a new re-encoded picture-> ts = "
										+ encodedPacket.getTimeStamp());
					}
					final int writePacketRetCode = outputContainer
							.writePacket(encodedPacket);

					if (writePacketRetCode != 0) {
						final IError err = IError.make(writePacketRetCode);
						H263toSorenson.log
								.error("IContainer.writePacket() returned an error: "
										+ err.getType()
										+ ", "
										+ err.getDescription());
					}

					final int flushCode = outputContainer.flushPackets();
					if (flushCode < 0) {
						H263toSorenson.log.warn("Flush NOK!!!");
					}

					// ts = encodedPacket.getTimeStamp()*10;
					ts = currentRTPTimestamp;

					final byte[] data = myHandler.getLastPacket();
					final byte[] frame = new byte[data.length - 11 - 4];

					System.arraycopy(data, 11, frame, 0, frame.length);

					if (index < H263toSorenson.LOG_INDEX_MAX) {
						H263toSorenson.log
								.debug("Sorenson frame ->  ts (from RTP packet) = "
										+ ts);
						Util.display20Hexa(frame, "Sorenson frame", (int) ts,
								H263toSorenson.class, true);
					}
					// finally push timestamp and data to rtmpStreamSender
					rtmpStreamSender.pushSorensonVideoData(ts, frame);
					previousTimestamp = currentTimestamp;
				} else {
					VideoTimer.setFrameEncodingTimeFail();
					H263toSorenson.log
							.warn("Incomplete RE-ENCODED Sorenson packet -> dropped");
				}
			} else {
				VideoTimer.setFrameDecodingTimeFail();
				H263toSorenson.log.warn("Incomplete H263 packet dropped");
			}
			if ((System.nanoTime() - computationTime) / 1000000 > 50) {
				Float ratio = 100 * (float) counter / (index + 1);
				H263toSorenson.log.debug("Computation time(n°" + index + ") = "
						+ (System.nanoTime() - computationTime) / 1000000
						+ " ms ratio " + ratio.toString() + " %");
				counter++;
			}
			index++;
		}

		int flushEncoderRetCode;
		do {
			flushEncoderRetCode = encoder.encodeVideo(encodedPacket, null, -1);
			H263toSorenson.log
					.info("Flushed a packet through the video encoder, which returned "
							+ flushEncoderRetCode);
			if (encodedPacket.isComplete()) {
				H263toSorenson.log.info("The flushed packet is complete");
			} else {
				H263toSorenson.log.info("The flushed packet is incomplete");
			}
		} while (flushEncoderRetCode >= 0 && encodedPacket.isComplete());

		final int cwtRetCode = outputContainer.writeTrailer();
		if (cwtRetCode != 0) {
			final IError err = IError.make(cwtRetCode);
			H263toSorenson.log
					.error("IContainer.writeHeader() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return;
		}

		encoder.close();
		encoder = null;

		final int closeRetcode = outputContainer.close();
		if (closeRetcode >= 0) {
			H263toSorenson.log.info("Video outputContainer: Closed");
		} else {
			H263toSorenson.log.warn("Video outputContainer: NOT Closed !!!!!");
		}
		outputContainer = null;

		myThread = null;

		H263toSorenson.log.info("Video (Xuggle) transcoding: stopped");

	}

	public void halt() {
		running = false;
		if (myThread != null) {
			H263toSorenson.log.info("Interruption -> name "
					+ myThread.getName() + " Id " + myThread.getId());
			myThread.interrupt();
		}
	}

	private boolean closeCurrentOutputContainer(IStreamCoder encoder) {
		int flushEncoderRetCode;
		do {
			flushEncoderRetCode = encoder.encodeVideo(encodedPacket, null, -1);
			H263toSorenson.log
					.info("Flushed a packet through the video encoder, which returned "
							+ flushEncoderRetCode);
			if (encodedPacket.isComplete()) {
				H263toSorenson.log.info("The flushed packet is complete");
			} else {
				H263toSorenson.log.info("The flushed packet is incomplete");
			}
		} while (flushEncoderRetCode >= 0 && encodedPacket.isComplete());

		final int cwtRetCode = outputContainer.writeTrailer();
		if (cwtRetCode != 0) {
			final IError err = IError.make(cwtRetCode);
			H263toSorenson.log
					.error("IContainer.writeHeader() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return false;
		}

		encoder.close();
		encoder = null;

		final int closeRetcode = outputContainer.close();
		if (closeRetcode >= 0) {
			H263toSorenson.log.info("Video outputContainer: Closed");
		} else {
			H263toSorenson.log.warn("Video outputContainer: NOT Closed !!!!!");
		}
		outputContainer = null;
		return true;
	}

	private boolean setOutputContainer() {

		// Create and configure the container format object
		final IContainerFormat transcodedContainerFormat = IContainerFormat
				.make();
		final int cfRetCode = transcodedContainerFormat.setOutputFormat(
				H263toSorenson.containerFormatShortName, null, null);
		if (cfRetCode != 0) {
			final IError err = IError.make(cfRetCode);
			System.out
					.println("IContainerFormat.setOutputFormat() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return false;
		}
		H263toSorenson.log
				.info("Successfully set output format on IContainerFormat = "
						+ transcodedContainerFormat.getOutputFormatLongName());

		// Create and open the output container
		outputContainer = IContainer.make();

		final int cRetCode = outputContainer.open(myHandler,
				IContainer.Type.WRITE, transcodedContainerFormat, true, false);

		if (cRetCode != 0) {
			final IError err = IError.make(cRetCode);
			H263toSorenson.log.error("IContainer.open() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}
		H263toSorenson.log.info("Successfully opened output IContainer");

		outputContainer.addNewStream(0);

		encodedPacket = IPacket.make();

		return true;
	}

	private IStreamCoder getEncoder(final int width, final int height) {

		final IStreamCoder encoder = outputContainer.getStream(0)
				.getStreamCoder();
		encoder.setCodec(ICodec.ID.CODEC_ID_FLV1);

		H263toSorenson.log
				.info("Setting up the output stream coder for video...");

		encoder.setNumPicturesInGroupOfPictures(H263toSorenson.KEYRATE_SORENSON);
		encoder.setTimeBase(IRational.make(1, 1000));
		encoder.setPixelType(H263toSorenson.supportedOutputPixelType);
		encoder.setWidth(width);
		encoder.setHeight(height);
		final int encOpenRetVal = encoder.open();
		if (encOpenRetVal != 0) {
			final IError err = IError.make(encOpenRetVal);
			H263toSorenson.log.error("IStreamCoder.open() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return null;
		}
		return encoder;
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

		final int packetLength = packet.getData().length;
		if (index < 15) {
			H263toSorenson.log.debug("packet #" + index + " isKey "
					+ packet.isKey() + " ts = " + packet.getTimeStamp());
			Util.display20Hexa(packet.getData(), "H263+ frame",
					(int) packet.getTimeStamp(), H263toSorenson.class, true);
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

		if (index == 0) {
			firstRTPTimestamp = packet.getTimeStamp();
		}

		/*
		 * We allocate a new picture to get the data out of Xuggler
		 */

		final IVideoPicture picture = IVideoPicture
				.make(decoder.getPixelType(), decoder.getWidth(),
						decoder.getHeight());
		final IBuffer iBuffer = IBuffer.make(null, packet.getData(), 0,
				packetLength);
		final IPacket iPacket = IPacket.make(iBuffer);
		final int pksz = iPacket.getSize();
		iPacket.setComplete(true, pksz);
		iPacket.setTimeStamp(packet.getTimeStamp() - firstRTPTimestamp);
		// iPacket.setKeyPacket(packet.isKey());
		iPacket.setKeyPacket(true);
		if (index < 15) {
			H263toSorenson.log.debug("ipacket #" + index + " isKey "
					+ iPacket.isKey() + " ts = " + iPacket.getTimeStamp());
		}
		currentRTPTimestamp = iPacket.getTimeStamp() / 90;

		int offset = 0;
		while (offset < iPacket.getSize()) {
			/*
			 * Now, we decode the video, checking for any errors.
			 */
			final int bytesDecoded = decoder.decodeVideo(picture, iPacket,
					offset);
			if (bytesDecoded < 0) {
				// throw new RuntimeException("got error decoding video");
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
			}
		}
		VideoTimer.setFrameDecodingTime(System.nanoTime() / 1000,
				iPacket.getSize());
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

	private void setInputResampler(final int inputW, final int inputH,
			final int outputW, final int outputH) {
		inputResampler = null;
		inputResampler = IVideoResampler.make(outputW, outputH,
				IPixelFormat.Type.YUV420P, inputW, inputH,
				IPixelFormat.Type.YUV420P);

		if (inputResampler == null) {
			throw new RuntimeException("could not create color space resampler");
		}
	}

	private void logProperties() {
		String prop = null;
		IProperty metaProp = null;
		for (int i = 0; i < decoder.getNumProperties(); i++) {
			metaProp = decoder.getPropertyMetaData(i);
			prop = metaProp.getName();
			H263toSorenson.log.debug("Property #" + i + ": " + prop + " = "
					+ decoder.getPropertyAsString(prop) + " --- (default = "
					+ metaProp.getDefault() + ")");
		}
		H263toSorenson.log.debug("Auto stamps packets: "
				+ decoder.getAutomaticallyStampPacketsForStream());
		H263toSorenson.log.debug("Bit rate: " + decoder.getBitRate());
		H263toSorenson.log.debug("Bit rate tolerance: "
				+ decoder.getBitRateTolerance());
		H263toSorenson.log.debug("Channels: " + decoder.getChannels());
		H263toSorenson.log.debug("Codec: " + decoder.getCodec());
		H263toSorenson.log.debug("Codec ID: " + decoder.getCodecID());
		H263toSorenson.log.debug("Codec tag: " + decoder.getCodecTag());
		H263toSorenson.log.debug("Codec type: " + decoder.getCodecType());
		H263toSorenson.log.debug("Dimensions: " + decoder.getWidth() + "x"
				+ decoder.getHeight());
		H263toSorenson.log.debug("Direction: " + decoder.getDirection());
		H263toSorenson.log.debug("Global quality: "
				+ decoder.getGlobalQuality());
		H263toSorenson.log.debug("Pixel type: " + decoder.getPixelType());
		H263toSorenson.log.debug("Sample rate: " + decoder.getSampleRate());
		H263toSorenson.log.debug("Sample format: " + decoder.getSampleFormat());
		H263toSorenson.log.debug("Time base: " + decoder.getTimeBase());
	}

}