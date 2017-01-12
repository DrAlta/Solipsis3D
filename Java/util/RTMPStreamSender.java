package org.red5.server.webapp.sip;

import org.alcatelLucent.h264.depacketizer.CNal;
import org.apache.mina.core.buffer.IoBuffer;
import org.red5.logging.Red5LoggerFactory;
import org.red5.server.api.IContext;
import org.red5.server.api.IScope;
import org.red5.server.api.stream.IVideoStreamCodec;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.Notify;
import org.red5.server.net.rtmp.event.VideoData;
import org.red5.server.net.rtmp.message.Constants;
import org.red5.server.stream.BroadcastScope;
import org.red5.server.stream.IBroadcastScope;
import org.red5.server.stream.IProviderService;
import org.red5.server.stream.codec.AVCVideo;
import org.red5.server.stream.codec.StreamCodecInfo;
import org.slf4j.Logger;

public class RTMPStreamSender {
	private static final Logger log = Red5LoggerFactory.getLogger(
			RTMPStreamSender.class, "sip");
	private int logIndexAudio = 0;

	BroadcastStream outputStream = null;

	private IoBuffer audioBuffer = null;
	private IoBuffer videoBuffer = null;

	private byte[] keyFrameHeaderPart = { 0x17, 1, 0, 0, 0 };
	private byte[] interFrameHeaderPart = { 0x27, 1, 0, 0, 0 };

	private HeaderH264 header = null;
	private SpxEncoder encoder = null;

	protected final static String END_OF_VIDEO_STREAM = "END_OF_VIDEO_STREAM";
	protected final static String START_OF_VIDEO_STREAM = "START_OF_VIDEO_STREAM";
	protected final static String PAUSE = "PAUSE";
	protected final static String RESUME = "RESUME";

	// ----- Time stamps. -----

	public final static long TB_H264 = 90;

	private long offsetAudio = 0;
	private long firstAudioTS = 0;
	private long previousAudioTS = 0;
	private long audioTS = 0;
	private boolean isFirstAudio = true;

	private long offsetVideo = 0;
	private long firstVideoTS = 0;
	private long previousVideoTS = 0;
	private long videoTS = 0;
	private boolean isFirstVideo = true;
	private long firstVideoSendingTime = 0;
	private long videoSendingTime = 0;
	private final static long VIDEO_DELTA_DEFAULT = 67;

	private boolean hasNotifiedVideo = false;
	private int indexVideoFrame = 0;

	private volatile boolean ready = false;

	public RTMPStreamSender() {
		header = new HeaderH264();
		encoder = new SpxEncoder();
	}

	public void setup(String streamName, IScope scope, IoBuffer avcConf) {
		this.offsetAudio = 0;
		this.firstAudioTS = -1;
		this.previousAudioTS = 0;
		this.audioTS = 0;
		this.isFirstAudio = true;

		this.offsetVideo = 0;
		this.firstVideoTS = -1;
		this.previousVideoTS = 0;
		this.videoTS = 0;
		this.isFirstVideo = true;
		this.firstVideoSendingTime = 0;
		this.videoSendingTime = 0;

		this.hasNotifiedVideo = false;
		this.indexVideoFrame = 0;
		this.logIndexAudio = 0;

		StreamCodecInfo mCodecInfo = new StreamCodecInfo();
		mCodecInfo.setHasAudio(true);
		mCodecInfo.setHasVideo(false);
		if (avcConf != null) {
			mCodecInfo.setHasVideo(true);
			IVideoStreamCodec videoCodec = null;
			videoCodec = new AVCVideo();
			videoCodec.addData(avcConf);
			mCodecInfo.setVideoCodec(videoCodec);
		}

		outputStream = new BroadcastStream(streamName, mCodecInfo);
		outputStream.setPublishedName(streamName);
		outputStream.setScope(scope);

		IContext context = scope.getContext();
		IProviderService providerService = (IProviderService) context
				.getBean(IProviderService.BEAN_NAME);
		if (providerService.registerBroadcastStream(scope, streamName,
				outputStream)) {
			IBroadcastScope bsScope = (BroadcastScope) providerService
					.getLiveProviderInput(scope, streamName, true);
			bsScope.setAttribute(IBroadcastScope.STREAM_ATTRIBUTE, outputStream);
		} else {
			log.error("Got a fatal error; could not register broadcast stream");
		}

		Notify notify = new Notify();
		IoBuffer notifyBuffer = getSpeexAudioMetadata();
		notify.setData(notifyBuffer);
		outputStream.dispatchEvent(notify);
		ready = true;
	}

	public void startStream(String publishName) {
		log.debug("PublishName: " + publishName);
	}

	public void stopStream() {
		log.debug("Stream stopping...");
		this.outputStream = null;
		this.ready = false;
	}

	public void pushSpeexAudioData(float[] audio, int ts) {
		this.sleep(2000);
		if (ready) {
			audioBuffer = IoBuffer.allocate(60 + 1);
			audioBuffer.clear();
			audioBuffer.put((byte) -78);

			byte[] speexAudioData = null;
			if (encoder != null) {
				speexAudioData = encoder.encode(audio);
				audioBuffer.put(speexAudioData);
				audioBuffer.flip();
				AudioData audioData = new AudioData(audioBuffer);
				audioData.setTimestamp((int) this.setAudioTS(ts));
				outputStream.dispatchEvent(audioData);
				audioData.release();
			} else {
				log.error("SPEEX encoder is null !!! -> could not send audio data to Web client");
			}
		}
	}

	public void pushSpeexAudioData(byte[] audio, int ts) {
		this.sleep(2000);
		if (ready) {
			audioBuffer = IoBuffer.allocate(audio.length + 1);
			audioBuffer.clear();
			audioBuffer.put((byte) -78);
			audioBuffer.put(audio);
			audioBuffer.flip();

			AudioData audioData = new AudioData(audioBuffer);
			audioData.setTimestamp((int) this.setAudioTS(ts));
			outputStream.dispatchEvent(audioData);
			audioData.release();
		}
	}

	public void pushAVCConfiguration(IoBuffer avcConfBuffer) {
		this.sleep(2000);
		if (ready) {
			VideoData avcData = new VideoData();
			avcData.setDataType(Constants.TYPE_VIDEO_DATA);
			avcData.setData(avcConfBuffer);
			if (this.previousVideoTS <= 0) {
				avcData.setTimestamp((int) this.setVideoTS(0));
			} else {
				avcData.setTimestamp((int) this.previousVideoTS);
			}
			outputStream.dispatchEvent(avcData);
			avcData.release();
		}
	}

	/**
	 * UNUSED
	 * 
	 * @param data
	 *            the AVC configuration as a byte array
	 * @deprecated Please use
	 *             {@link RTMPStreamSender#pushAVCConfiguration(IoBuffer)}
	 *             instead.
	 */
	@Deprecated
	public void pushAVCConfiguration(byte[] data) {
		this.sleep(2000);
		if (ready) {
			IoBuffer avcConfBuffer = IoBuffer.allocate(data.length);
			avcConfBuffer.put(data);
			avcConfBuffer.flip();
			VideoData avcData = new VideoData();
			avcData.setDataType(Constants.TYPE_VIDEO_DATA);
			avcData.setData(avcConfBuffer);
			avcData.setTimestamp(0);
			outputStream.dispatchEvent(avcData);
			avcData.release();
		}
	}

	protected void pushVideoNotify() {
		long newTS;
		this.sleep(2000);
		if (ready) {
			newTS = this.setVideoTS(this.offsetAudio);
			Notify notify = new Notify();
			IoBuffer notifyBuffer = getH264VideoMetadata();
			notify.setData(notifyBuffer);
			notify.setTimestamp((int) newTS);
			outputStream.dispatchEvent(notify);
		}
	}

	public void pushSorensonVideoData(long ts, byte[] data) {
		// if (this.indexSorenson < 15)
		// log.debug("Pushing Sorenson frame #" + this.indexSorenson
		// + " at TS=" + ts);

		this.sleep(2000);
		long newTS;
		if (ready) {
			newTS = this.setVideoTS(ts);
			videoBuffer = IoBuffer.allocate(data.length);
			videoBuffer.clear();
			videoBuffer.put(data);
			videoBuffer.flip();
			VideoData flv1Data = new VideoData();
			flv1Data.setDataType(Constants.TYPE_VIDEO_DATA);
			flv1Data.setData(videoBuffer);
			flv1Data.setTimestamp((int) newTS);
			outputStream.dispatchEvent(flv1Data);
			flv1Data.release();

			if (this.indexVideoFrame < 15) {
				long difference = this.videoSendingTime - this.videoTS;
				if (difference == 0) {
					log.debug("Sending sorenson frame #" + this.indexVideoFrame
							+ " perfectly stamped");
				} else if (difference < 0) {
					log.debug("Sending sorenson frame #" + this.indexVideoFrame
							+ " roughly stamped: " + -difference + "ms late");
				} else {
					log.debug("Sending sorenson frame #" + this.indexVideoFrame
							+ " roughly stamped: " + difference + "ms early");
				}
				log.debug("Sorenson frame #" + this.indexVideoFrame
						+ " sent with TS=" + this.videoTS);
			}
			this.indexVideoFrame++;
		}
	}

	/**
	 * @deprecated Use {@link #pushVideoData(byte[],int,String,long,long)}
	 *             instead.
	 */
	@Deprecated
	public void pushVideoData(byte[] video, int length, String type,
			long currentTimestamp, long fps, boolean isFirstVideoPacket) {
		pushVideoData(video, length, type, currentTimestamp, fps);
	}

	public void pushVideoData(byte[] video, int length, String type,
			long currentTimestamp, long fps) {
		this.sleep(2000);
		if (ready) {
			this.setVideoTS(currentTimestamp / RTMPStreamSender.TB_H264);
			videoBuffer = IoBuffer.allocate(length + 9);
			videoBuffer.clear();
			if (type == CNal.KEY_FRAME) {
				// assuming KeyFrame
				videoBuffer.put(keyFrameHeaderPart);
				byte[] len = intToByteArray(length);
				videoBuffer.put(len);
			} else if (type == CNal.INTER_FRAME) {
				// assuming interFrame
				videoBuffer.put(interFrameHeaderPart);
				byte[] len = intToByteArray(length);
				videoBuffer.put(len);
			} else if ((byte) (video[0] & 0x1F) == 6) {
				log.info("nal_type=6 ts( " + currentTimestamp + " )= "
						+ this.videoTS + " fps = " + fps);
				return;
			} else if ((byte) (video[0] & 0x1F) == 7) {
				log.info("nal_type=7 ts( " + currentTimestamp + " )= "
						+ this.videoTS + " fps = " + fps);
				header.setSequenceParameterSet(video);
				return;
			} else if ((byte) (video[0] & 0x1F) == 8) {
				log.info("nal_type=8  ts( " + currentTimestamp + " )= "
						+ this.videoTS + " fps = " + fps);
				// displayByte(video, 8);
				// if (video.length > 10){
				// video[0] = 0x68;
				// video[1] = (byte)0xce;
				// video[2] = 0x3c;
				// video[3] = (byte)0x80;
				// }
				header.setPictureParameterSet(video);
				if (!this.hasNotifiedVideo) {
					this.hasNotifiedVideo = true;

					Notify notify = new Notify();
					if (fps == 0) {
						fps = 25;
					}
					IoBuffer notifyBuffer = getH264VideoMetadata((int) fps);
					notify.setData(notifyBuffer);
					notify.setTimestamp((int) this.videoTS);
					outputStream.dispatchEvent(notify);
					IoBuffer headerH264Buffer = header.getHeaderH264();
					if (headerH264Buffer == null) {
						log.warn("couldn't send AVC configuration: nal type =7 not received...");
						return;
					}

					VideoData avcConfig = new VideoData();
					avcConfig.setDataType(Constants.TYPE_VIDEO_DATA);
					avcConfig.setData(headerH264Buffer);
					avcConfig.setTimestamp((int) this.videoTS);
					outputStream.dispatchEvent(avcConfig);
				} else {
					IoBuffer headerH264Buffer = header.getHeaderH264();
					if (headerH264Buffer == null) {
						log.warn("couldn't send AVC configuration: nal type =7 not received...");
						return;
					}
					VideoData data = new VideoData();
					data.setDataType(Constants.TYPE_VIDEO_DATA);
					data.setData(headerH264Buffer);
					data.setTimestamp((int) this.videoTS);
					outputStream.dispatchEvent(data);
				}
				return;
			}

			videoBuffer.put(video);
			videoBuffer.flip();
			VideoData videoData = new VideoData();
			videoData.setDataType(Constants.TYPE_VIDEO_DATA);
			videoData.setData(videoBuffer);
			videoData.setTimestamp((int) this.videoTS);
			outputStream.dispatchEvent(videoData);
			videoData.release();
			indexVideoFrame++;

			if ((float) indexVideoFrame / (float) 50 == indexVideoFrame / 50) {
				log.debug("fps = " + fps);
			}
		}
	}

	private IoBuffer getH264VideoMetadata() {
		MetaDataWriter meta = new MetaDataWriter();
		IoBuffer notifyBuffer = null;
		Double duration = -1d;
		Integer videoCodecId = 7;
		Integer width = 355;
		Integer height = 288;
		Integer videoframerate = null;
		Integer avcprofile = null;
		Integer avclevel = null;

		notifyBuffer = meta.writeVideoMetadataTag(duration, videoCodecId,
				width, height, videoframerate, avcprofile, avclevel);
		return notifyBuffer;
	}

	private IoBuffer getH264VideoMetadata(Integer fps) {
		MetaDataWriter meta = new MetaDataWriter();
		IoBuffer notifyBuffer = null;
		Double duration = -1d;
		Integer videoCodecId = 7;
		Integer width = 355;
		Integer height = 288;
		Integer videoframerate = fps;
		Integer avcprofile = 66;
		Integer avclevel = 1;

		notifyBuffer = meta.writeVideoMetadataTag(duration, videoCodecId,
				width, height, videoframerate, avcprofile, avclevel);
		return notifyBuffer;
	}

	protected static IoBuffer getSpeexAudioMetadata() {
		MetaDataWriter meta = new MetaDataWriter();
		IoBuffer notifyBuffer = null;

		Double duration = -1d;
		Integer audioCodecId = 11;
		Integer audiosamplerate = 8000;
		Integer audiochannels = 1;

		notifyBuffer = meta.writeAudioMetadataTag(duration, audioCodecId,
				audiosamplerate, audiochannels);
		return notifyBuffer;
	}

	protected static IoBuffer getSpeexAudioH264VideoMetadata() {
		IoBuffer notifyBuffer = null;
		MetaDataWriter meta = new MetaDataWriter();
		notifyBuffer = meta.writeSpeexH264MetadataTag();
		return notifyBuffer;
	}

	public static byte[] intToByteArray(int value) {
		byte[] b = new byte[4];
		for (int i = 0; i < 4; i++) {
			int offset = (b.length - 1 - i) * 8;
			b[i] = (byte) (value >>> offset & 0xFF);
		}
		return b;
	}

	private void sleep(int max) {
		int step = 100;
		int indexMax = max / step;
		for (int i = 0; i < indexMax; i++) {
			if (ready) {
				return;
			} else {
				try {
					Thread.sleep(step);
				} catch (InterruptedException e) {
					log.error(e.getMessage());
				}
			}
		}
	}

	// ---------- Time stamps managing. ----------

	private long setAudioTS(long ts) {
		if (this.isFirstAudio) {
			log.info("First audio packet received, calculating time stamps...");
			this.firstAudioTS = ts;
			if (!this.isFirstVideo) {
				this.offsetVideo = this.videoTS;
				log.info("Video was already started, setting offset to "
						+ this.offsetVideo);
			}
		}
		this.audioTS = ts - this.firstAudioTS + this.offsetVideo;
		if (!this.isFirstAudio) {
			if (this.audioTS < this.previousAudioTS) {
				log.error("Bad audio TS: must not be decreasing! rectifying...");
				this.audioTS = this.previousAudioTS;
			} else if (this.audioTS == this.previousAudioTS) {
				log.warn("Identical successive audio TS: " + this.audioTS);
			} else if (this.logIndexAudio < 15) {
				log.debug("Audio delta="
						+ (this.audioTS - this.previousAudioTS));
			}
		}
		this.previousAudioTS = this.audioTS;
		if (this.audioTS < 0) {
			log.warn("Negative audio TS: " + this.audioTS);
		}
		if (this.isFirstAudio) {
			this.isFirstAudio = false;
		}
		this.logIndexAudio++;
		return this.audioTS;
	}

	private long setVideoTS(long ts) {
		if (this.isFirstVideo) {
			log.info("First video packet received, calculating time stamps...");
			this.firstVideoTS = ts;
			this.firstVideoSendingTime = System.currentTimeMillis();
			if (!this.isFirstAudio) {
				this.offsetAudio = this.audioTS;
				this.firstVideoSendingTime -= this.offsetAudio;
				log.info("Audio was already started, setting offset to "
						+ this.offsetAudio);
			}
		}
		this.videoTS = ts - this.firstVideoTS + this.offsetAudio;
		if (!this.isFirstVideo) {
			if (this.videoTS < this.previousVideoTS) {
				log.error("Bad video TS on frame #" + this.indexVideoFrame
						+ ": must not be decreasing! (" + this.previousVideoTS
						+ " then " + this.videoTS + ") rectifying...");
				this.videoTS = this.previousVideoTS;
			} else if (this.videoTS == this.previousVideoTS) {
				log.warn("Identical successive video TS on frame #"
						+ this.indexVideoFrame + ": " + this.videoTS);
			} else {
				long delta = this.videoTS - this.previousVideoTS;
				if (this.indexVideoFrame < 15) {
					log.debug("Video delta=" + delta);
				}
				this.checkVideoDelta(delta);
			}
		}
		this.previousVideoTS = this.videoTS;
		if (this.videoTS < 0) {
			log.warn("Negative video TS: " + this.videoTS);
		}
		if (this.isFirstVideo) {
			this.isFirstVideo = false;
		}
		this.videoSendingTime = System.currentTimeMillis()
				- this.firstVideoSendingTime;
		return this.videoTS;
	}

	private void checkVideoDelta(long delta) {
		if (delta < RTMPStreamSender.VIDEO_DELTA_DEFAULT / 4
				|| delta > RTMPStreamSender.VIDEO_DELTA_DEFAULT * 4) {
			log.warn("Bad video delta on frame #" + this.indexVideoFrame + ": "
					+ delta + "ms between " + this.previousVideoTS + " and "
					+ this.videoTS + " ; setting "
					+ RTMPStreamSender.VIDEO_DELTA_DEFAULT + "ms instead");
			long newOffset = delta - RTMPStreamSender.VIDEO_DELTA_DEFAULT;
			this.firstVideoSendingTime += newOffset;
			this.firstVideoTS += newOffset;
			this.previousVideoTS -= newOffset;
			this.videoTS -= newOffset;
		}
	}

}
