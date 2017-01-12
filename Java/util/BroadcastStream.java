package org.red5.server.webapp.sip;

import java.io.File;
import java.io.IOException;
import java.util.Collection;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

import org.apache.mina.core.buffer.IoBuffer;
import org.red5.io.flv.impl.FLV;
import org.red5.io.flv.impl.FLVService;
import org.red5.io.flv.impl.FLVWriter;
import org.red5.io.flv.impl.Tag;
import org.red5.logging.Red5LoggerFactory;
import org.red5.server.api.IConnection;
import org.red5.server.api.IScope;
import org.red5.server.api.Red5;
import org.red5.server.api.stream.IBroadcastStream;
import org.red5.server.api.stream.IStreamCodecInfo;
import org.red5.server.api.stream.IStreamListener;
import org.red5.server.api.stream.ResourceExistException;
import org.red5.server.api.stream.ResourceNotFoundException;
import org.red5.server.messaging.IMessageComponent;
import org.red5.server.messaging.IPipe;
import org.red5.server.messaging.IPipeConnectionListener;
import org.red5.server.messaging.IProvider;
import org.red5.server.messaging.OOBControlMessage;
import org.red5.server.messaging.PipeConnectionEvent;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.IRTMPEvent;
import org.red5.server.net.rtmp.event.Notify;
import org.red5.server.net.rtmp.event.VideoData;
import org.red5.server.stream.IStreamData;
import org.red5.server.stream.codec.SorensonVideo;
import org.red5.server.stream.codec.StreamCodecInfo;
import org.red5.server.stream.message.RTMPMessage;
import org.slf4j.Logger;
import org.zoolu.sip.provider.SipStack;

public class BroadcastStream implements IBroadcastStream, IProvider,
		IPipeConnectionListener {
	/** Listeners to get notified about received packets. */
	private Set<IStreamListener> mListeners = new CopyOnWriteArraySet<IStreamListener>();
	final private Logger log = Red5LoggerFactory.getLogger(this.getClass(),
			"sip");

	private String mPublishedName;
	private IPipe mLivePipe;
	private IScope mScope;

	// Codec handling stuff for frame dropping
	private StreamCodecInfo mCodecInfo;
	private Long mCreationTime;
	private Notify metaData = null;

	FLVWriter writer = null;

	File myfile = null;
	boolean recording = false;
	
	int firstTimestampAudio = 0;
	boolean firstAudioData = true;

	private int kta = 0;
	private int ktv = 0;


	public BroadcastStream(String name,StreamCodecInfo streamCodecInfo) {
		mPublishedName = name;
		mLivePipe = null;
		log.info("Publish name (server side): {}", name);
		if (streamCodecInfo == null){
			mCodecInfo = new StreamCodecInfo();
			mCodecInfo.setVideoCodec(new SorensonVideo());
		}else{
			mCodecInfo = streamCodecInfo;
		}	
		mCreationTime = System.currentTimeMillis();
		recording = false;
	}
	

	private void setWriter() {
		log.debug("setWriter starting...");
		if (myfile != null) {
			if (myfile.exists()) {
				log.info("record : " + myfile.getName() + " already exists");
				return;
			}
		} else {
			String appPath = System.getProperty("user.dir");
			log.debug("appPath" + appPath);
			if (appPath.endsWith("wrapper")) {
				// removing /wrapper sub folder from path
				appPath = appPath.substring(0, (appPath.length() - 8));
			}
			// String name =
			// mPublishedName.substring(mPublishedName.indexOf(":")+1);
			IConnection conn = Red5.getConnectionLocal();
			String name = "video";
			Long tsServer = new Long(System.currentTimeMillis());
			log.debug("tsServer" + tsServer);
			String suffix = tsServer.toString().substring(
					tsServer.toString().length() - 4);
			log.debug("suffix" + suffix);
			if (conn != null) {
				name += "_" + conn.getClient().getId() + "_" + suffix;
			} else {
				name += "_" + suffix;
			}
			String fileName = appPath + "/webapps/sip/streams/" + name + ".flv";
			log.info("record : " + fileName);
			myfile = new File(fileName);
			try {
				myfile.createNewFile();
			} catch (IOException e) {
				log.error(e.getMessage());
				return;
			}
			FLVService flvService = new FLVService();
			flvService.setGenerateMetadata(false);
			FLV flv;
			try {
				flv = (FLV) flvService.getStreamableFile(myfile);
			} catch (IOException e) {
				log.error(e.getMessage());
				return;
			}
			try {
				writer = (FLVWriter) flv.getWriter();
			} catch (IOException e) {
				log.error(e.getMessage());
				return;
			}
		}
	}

	public IProvider getProvider() {
		log.info("getProvider()");
		return this;
	}

	public String getPublishedName() {
		log.info("getPublishedName()");
		return mPublishedName;
	}

	public String getSaveFilename() {
		log.error("getSaveFilename()");
		throw new Error("unimplemented method");
	}

	public void addStreamListener(IStreamListener listener) {
		log.info("addStreamListener(listener: {})", listener);
		mListeners.add(listener);
	}

	public Collection<IStreamListener> getStreamListeners() {
		// log.debug("getStreamListeners()");
		return mListeners;
	}

	public void removeStreamListener(IStreamListener listener) {
		log.info("removeStreamListener({})", listener);
		mListeners.remove(listener);
	}

	public void saveAs(String filePath, boolean isAppend) throws IOException,
			ResourceNotFoundException, ResourceExistException {
		log.error("saveAs(filepath:{}, isAppend:{})", filePath, isAppend);
		throw new Error("unimplemented method");
	}

	public void setPublishedName(String name) {
		log.info("setPublishedName(name:{})", name);
		mPublishedName = name;
	}

	public void close() {
		log.info("close()");
	}

	public IStreamCodecInfo getCodecInfo() {
		// log.debug("getCodecInfo()");
		return mCodecInfo;
	}

	public String getName() {
		log.info("getName(): {}", mPublishedName);
		return mPublishedName;
	}

	public void setScope(IScope scope) {
		mScope = scope;
	}

	public IScope getScope() {
		log.info("getScope(): {}", mScope);
		return mScope;
	}

	public void start() {
		log.info("start() nothing to do !!!");
	}

	public void stop() {
		log.info("stop");
	}

	public void onOOBControlMessage(IMessageComponent source, IPipe pipe,
			OOBControlMessage oobCtrlMsg) {
		log.info("onOOBControlMessage");
	}

	public void onPipeConnectionEvent(PipeConnectionEvent event) {
		log.info("onPipeConnectionEvent(event:{})", event);
		switch (event.getType()) {
		case PipeConnectionEvent.PROVIDER_CONNECT_PUSH:
			if (event.getProvider() == this
					&& (event.getParamMap() == null || !event.getParamMap()
							.containsKey("record"))) {
				this.mLivePipe = (IPipe) event.getSource();
			}
			break;
		case PipeConnectionEvent.PROVIDER_DISCONNECT:
			if (this.mLivePipe == event.getSource()) {
				this.mLivePipe = null;
			}
			break;
		case PipeConnectionEvent.CONSUMER_CONNECT_PUSH:
			break;
		case PipeConnectionEvent.CONSUMER_DISCONNECT:
			log.info("CONSUMER_DISCONNECT");
			break;
		default:
			break;
		}
	}

	public synchronized void dispatchEvent(Object event) {
		// log.debug("dispatchEvent(event:{})", event);
		if (event instanceof String) {
			String msg = (String) event;
			if (msg.equals(RTMPStreamSender.START_OF_VIDEO_STREAM)) {
				recording = true;
				log.info("Video Record starting: "
						+ RTMPStreamSender.START_OF_VIDEO_STREAM);
			} else if (msg.equals(RTMPStreamSender.END_OF_VIDEO_STREAM)) {
				recording = false;
				if (writer != null) {
					log.info("Video Record ended: "
							+ RTMPStreamSender.END_OF_VIDEO_STREAM);
					writer.close();
					writer = null;
				}
			} else if (msg.equals(RTMPStreamSender.PAUSE)) {
				recording = false;
			} else if (msg.equals(RTMPStreamSender.RESUME)) {
				recording = true;
			}
		}else if (event instanceof Notify) {
			Notify notify = (Notify) event;
			//metaData = notify;
			log.info("push notify to client: type = " + notify.getDataType());
		}
		if (event instanceof IRTMPEvent) {
			int ts = 0;
			IRTMPEvent rtmpEvent = (IRTMPEvent) event;
			if (mLivePipe != null) {
				RTMPMessage msg = new RTMPMessage();

				msg.setBody(rtmpEvent);
				if (mCreationTime == null) {
					// mCreationTime = (long) rtmpEvent.getTimestamp();
					mCreationTime = System.currentTimeMillis();
				} else {
					ts = (int) ((System.currentTimeMillis() - mCreationTime));
				}

				if (!(rtmpEvent instanceof IStreamData)) {
					log.info("skipping non stream data");
					return;
				}
				try {
					if (event instanceof AudioData) {
						AudioData audioData = (AudioData) event;
						mCodecInfo.setHasAudio(true);
						// audioTs += audioData.getTimestamp();
						int audioTs = audioData.getTimestamp();
						// audioData.setTimestamp(audioTs);
						if (kta < 15) {
							log.info("Push rtmp message (audio) Timestamp "
									+ audioTs + " ts (server) " + ts);
						}
						kta++;
					} else if (event instanceof VideoData) {
						VideoData videoData = (VideoData) event;
						mCodecInfo.setHasVideo(true);
						int videoTs = (videoData.getTimestamp());
						int length = videoData.getData().limit();
						if (ktv < 15) {
							log.info("Push rtmp message (L= " + length
									+ " ) (video) Timestamp " + videoTs
									+ " ts (server) " + ts);
							Util.display20Hexa(videoData.getData().array(),
									"time", videoTs, BroadcastStream.class,
									true);
						}
						ktv++;
					}
					if (SipStack.h264_videoRecording) {
						// if (event instanceof VideoData || event instanceof
						// AudioData){
						if (event instanceof VideoData) {
							if (recording) {
								Tag tag = new Tag();
								IRTMPEvent body = msg.getBody();
								tag.setDataType(body.getDataType());
								if (event instanceof AudioData
										&& firstAudioData) {
									firstTimestampAudio = body.getTimestamp();
									log.info("Reseting audio timestamp for file recording: "
											+ body.getTimestamp());
									firstAudioData = false;
								}
								if (event instanceof AudioData) {
									tag.setTimestamp(body.getTimestamp()
											- firstTimestampAudio);
								} else {
									tag.setTimestamp(body.getTimestamp());
								}
								if (body instanceof IStreamData) {
									IoBuffer data = ((IStreamData) body)
											.getData().asReadOnlyBuffer();
									tag.setBodySize(data.limit());
									tag.setBody(data);
									// not working
									// Util.display20Bytes(tag.getBody().array(),
									// tag.getTimestamp(),BroadcastStream.class);
									// Util.display20Hexa(tag.getBody().array(),
									// tag.getTimestamp(),BroadcastStream.class);
								}
								if (writer == null) {
									setWriter();
								}
								try {
									if (writer != null)
										writer.writeTag(tag);
								} catch (IOException e) {
									log.error("Error writing tag ", e);
								}
							}
						}
					}
					mLivePipe.pushMessage(msg);				
				} catch (IOException ex) {
					log.error("Got exception: {}", ex);
				}
			}
		}
	}

	// @Override
	public long getCreationTime() {
		log.debug("mCreationTime" + mCreationTime);
		return mCreationTime;
	}

	// @Override
	public Notify getMetaData() {
		log.debug("return Notify ...");
		if (metaData == null){
			metaData = new Notify();
			if (mCodecInfo.hasVideo()){
				IoBuffer notifyBuffer = RTMPStreamSender.getSpeexAudioH264VideoMetadata();
				metaData.setData(notifyBuffer);
				metaData.setTimestamp(0);
			}else{
				IoBuffer notifyBuffer = RTMPStreamSender.getSpeexAudioMetadata();
				metaData.setData(notifyBuffer);
				metaData.setTimestamp(0);
			}
		}
		return metaData;
	}

}
