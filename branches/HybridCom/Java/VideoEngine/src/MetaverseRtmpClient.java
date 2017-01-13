

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Map;
import java.net.*;


import javax.imageio.ImageIO;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;

import org.apache.mina.common.ByteBuffer;
import org.red5.io.utils.ObjectMap;
import org.red5.server.api.service.IPendingServiceCall;
import org.red5.server.api.service.IPendingServiceCallback;
import org.red5.server.net.rtmp.INetStreamEventHandler;
import org.red5.server.net.rtmp.RTMPClient;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.Notify;
import org.red5.server.net.rtmp.event.VideoData;
import org.red5.server.net.rtmp.status.StatusCodes;
import org.red5.server.stream.message.RTMPMessage;

import com.xuggle.xuggler.ICodec;
import com.xuggle.xuggler.ICodec.ID;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;
import com.xuggle.xuggler.IError;
import com.xuggle.xuggler.IPacket;
import com.xuggle.xuggler.IPixelFormat;
import com.xuggle.xuggler.IRational;
import com.xuggle.xuggler.IStream;
import com.xuggle.xuggler.IStreamCoder;
import com.xuggle.xuggler.IVideoPicture;
import com.xuggle.xuggler.video.ConverterFactory;
import com.xuggle.xuggler.video.IConverter;


public class MetaverseRtmpClient implements IPendingServiceCallback, INetStreamEventHandler{

	protected static final int ERROR = -1;
	protected static final int INITIAL = 0;
	protected static final int CREATED = 1;
	protected static final int CONNECTED = 2;
	protected static final int REGISTERED = 3;
	protected static final int ON_CALL = 4;
	protected static final int PUBLISHING = 5;
	protected static final int FINISH = 6;

	public static String HOST = "localhost";
	public static int PORT = 1935;
	public static String APP = "vhc";
	public static int CLIENT_NUMBER = 1;

	public String host = MetaverseRtmpClient.HOST;
	public int port = MetaverseRtmpClient.PORT;
	public String app = MetaverseRtmpClient.APP;
	public int clientNumber = MetaverseRtmpClient.CLIENT_NUMBER;
	public String metaverse_home = null; 

	private String code;
	private String description;
	private RTMPClient rtmpClient;
	private int streamId;
	protected int state = INITIAL;
	protected String errorMessage = null;
	private IPendingServiceCallback callBackProvider;

	public static String USERNAME = "flex";
	public static String PASSWORD = "flex";
	public static String REALM = "comica.net";
	public static String PROXY = "comica.net";

// TB used in SIP commuication 
	protected String username = USERNAME;
	private String password = PASSWORD;
	private String realm = REALM;
	private String proxy = PROXY;

	byte[] buf = new byte[1024];
	byte[] ebuf = new byte[1024];
	int portSolipsisRecv = 8974;
	int portSolipsisSend = 8976;
	DatagramSocket socket = null;
	DatagramSocket socket1 = null;
	int viewToggle = 0;
	InetAddress solipsisHost = null;
	DatagramPacket reception = null;
	DatagramPacket emission = null;
	BufferedImage loadImg = null;
	long firstTimeStamp = 0;
	
	IContainer outputContainer = null;
	IContainerFormat containerFormat = null;
	IStream outStream = null;
	IStreamCoder outStreamCoder = null;
	ICodec codec = null;
	MyProtocolHandler myHandler = null;
	public static final int outWidth = 400;
	public static final int outHeight = 240;
	IPacket encodedPacket;
	
	static String containerFormatShortName = "flv";
	static ID codecID = ICodec.ID.CODEC_ID_FLV1;
	static IPixelFormat.Type supportedOutputPixelType = IPixelFormat.Type.YUV420P;
	IContainerFormat encodedContainerFormat;
	
	protected String callee = null;
	String publishName = "met";
	
	ArrayList<byte[]> list = null;

	public MetaverseRtmpClient() {	

		rtmpClient = new RTMPClient();
		String name = Thread.currentThread().getName();
				
		rtmpClient.setServiceProvider(this);
		callBackProvider = this;
		this.state = CREATED;
		
	}

	public void setCallbackProvider(IPendingServiceCallback value)
	{
		callBackProvider = value;
	}

	public void setHost(String value)
	{
		host = value;
	}

	public void setPort(int value)
	{
		port = value;
	}

	public void setApp(String value)
	{
		app = value;
	}

	public void setClientNumber(int value)
	{
		clientNumber = value;
	}

	public int getState()
	{
		return state;
	}


	public void setUsername(String username) {
		this.username = username;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public void setRealm(String realm) {
		this.realm = realm;
	}

	public void setProxy(String proxy) {
		this.proxy = proxy;
	}

	synchronized public void onStreamEvent(Notify notify) {
		System.out.println("onStreamEvent " + notify);

		ObjectMap map = (ObjectMap) notify.getCall().getArguments()[0];
		String code = (String) map.get("code");
	}

	public INetStreamEventHandler publishCallback(){
		System.out.println("publishCallback");
		return null;
	}

	synchronized public void resultReceived(IPendingServiceCall call) {
		Object result = call.getResult();

		if (call.getServiceMethodName().equals("connect")) {
			if (result instanceof ObjectMap) {
				ObjectMap map = (ObjectMap) result;
				code = (String) map.get("code");
				description = (String) map.get("description");

				if (StatusCodes.NC_CONNECT_SUCCESS.equals(code)) {
					state = CONNECTED;
					System.out.println(username + " is connected");
					rtmpClient.createStream(this);
				} else if (StatusCodes.NC_CONNECT_FAILED.equals(code)) {
					System.out.println(username
							+ " is NOT connected: NC_CONNECT_FAILED");
					rtmpClient.disconnect();
					state = ERROR;
					errorMessage = code;
				} else if (StatusCodes.NC_CONNECT_REJECTED.equals(code)) {
					System.out.println(username
							+ " is NOT connected: NC_CONNECT_REJECTED");
					rtmpClient.disconnect();
					state = ERROR;
					errorMessage = code;
				} else if (StatusCodes.NC_CONNECT_INVALID_APPLICATION
						.equals(code)) {
					System.out.println(username
							+ " is NOT connected: NC_CONNECT_INVALID_APPLICATION");
					rtmpClient.disconnect();
					state = ERROR;
					errorMessage = code;
				}
			}
		} else if (call.getServiceMethodName().equals("open")) {
			System.out.println(username + " OPEN: result = " + call.getResult() + " status = " + call.getStatus());
		} else if (call.getServiceMethodName().equals("call")) {
			System.out.println(username + "CALL: result = " + call.getResult() + " status = " + call.getStatus());
		} else if (call.getServiceMethodName().equals("createStream")) {
			if (result instanceof Integer) {
				Integer streamIdInt = (Integer) result;
				streamId = streamIdInt.intValue();
				rtmpClient.publish(streamId, publishName, "live",
						publishCallback());
				System.out.println(username + " outgoing stream created");
				state = PUBLISHING;
			}
		} else if (call.getServiceMethodName().equals("hangup")) {
			System.out.println(username + "HANGUP: result = " + call.getResult() + " status = " + call.getStatus());
		} else if (call.getServiceMethodName().equals("close")) {
			System.out.println(username + "CLOSE: result = " + call.getResult() + " status = " + call.getStatus());
		} 
	}

	public void onStatus(Object obj) {
	}
	
	public  RTMPMessage makeVideoPacket(IVideoPicture newpic) throws IOException {

		byte [] data = null;
		byte [] frame = null;
		
		final int encodeVideoRetCode = outStreamCoder.encodeVideo(
				encodedPacket, newpic, newpic.getSize() * 2);
		if (encodeVideoRetCode < 0) {
			final IError err = IError.make(encodeVideoRetCode);
			System.out.println ("======IStreamCoder.encodeVideo() returned an error: ======="
							+ err.getType()
							+ ", "
							+ err.getDescription());
		}

		// Write encoded packet to output Container
		if (encodedPacket.isComplete()) {
			final int writePacketRetCode = outputContainer.writePacket(encodedPacket);

			if (writePacketRetCode != 0) {
				final IError err = IError.make(writePacketRetCode);
				System.out.println("======IContainer.writePacket() returned an error: ======"
								+ err.getType()
								+ ", "
								+ err.getDescription());
			}

			final int flushCode = outputContainer.flushPackets();
			if (flushCode < 0) {
				System.out.println("======Flush NOK!!!======");
			}

			data = myHandler.getLastPacket();
			if (data.length < 20){
				System.out.println("======data length < 20!======");
				return null;
			}
			frame = new byte[data.length - 11 - 4];

			System.arraycopy(data, 11, frame, 0, frame.length);
		}
		else {
			System.out.println("======packet not completed!!!======");
			return null;
		}
		VideoData videoData = new VideoData();
		videoData.setData(frame);
		RTMPMessage rtmpMsg = new RTMPMessage();
		rtmpMsg.setBody(videoData);
		return rtmpMsg;
	}

	public void setStream(int idx) {
		System.out.println("I should do something here... " + idx);
		viewToggle = idx;
	}
	
	public void runVideoAndPush() throws InterruptedException {
		RTMPMessage rtmpMsg = null;
		String filename;
		
		//rtmpClient.connect(host, port, app, callBackProvider);
		
		rtmpClient.connect(
				host,
				port,
				rtmpClient.makeDefaultConnectionParams(host, port, app),
				callBackProvider,
				new Object[] {"vhcCtrl"});
		
		System.out.println(username + " connect sending ...");

		File lockf = null;
		String lockname = this.metaverse_home + "metaverseVideo_lock";
		lockf = new File(lockname);
		System.out.println("lockf = " + lockname);
		try {
			lockf.createNewFile();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			System.out.println("lockf create failed");
		}
				
		System.out.println("lockf success");
		
		String os = System.getProperty("os.name");
		System.out.println("os name = " + os);
		
		while (state != ERROR && state != FINISH) {
			Thread.sleep(60);
			while (state == PUBLISHING){
				try {
					System.out.println("wait for a new file");
					socket.receive(reception);
					System.out.println("ok receive a file");
				}
				catch (IOException e){
					System.out.println("receive socket error");
				}
				filename=new String(buf, 0, reception.getLength());
				System.out.println("received " + reception.getLength() + " bytes " + "filename: " + filename);
				File f = new File(filename);
				try {
					loadImg = ImageIO.read(f);
				}
				catch (IOException e){
					System.out.println("=======read file error=======");
					continue;
				}
				BufferedImage xugBufferedImage = convertToType(loadImg,
						BufferedImage.TYPE_3BYTE_BGR);
				IConverter converter = ConverterFactory.createConverter(xugBufferedImage,
						IPixelFormat.Type.YUV420P);

				long now = System.currentTimeMillis(); 
				if (firstTimeStamp == -1) 
					firstTimeStamp = now;
				long timeStamp = (now - firstTimeStamp)*1000; // convert to microseconds 
				
				IVideoPicture pic = converter.toPicture(xugBufferedImage, timeStamp); 
				f.delete();
				System.out.println("pic ok f delete");
				
				try {
					rtmpMsg = makeVideoPacket(pic);
					if (rtmpMsg == null){
						System.out.println("rtmpMsg null");
						continue;
					}
				} catch (IOException e) {
					e.printStackTrace();
				}

				rtmpClient.publishStreamData(streamId, rtmpMsg);
				System.out.println("rtmpMsg has been sent");
				
				String s = "";
				if (viewToggle == 0)
					 s = "view0";
				else
					 s = "view1";
				try {
					ebuf = s.getBytes("US-ASCII");
				} catch (UnsupportedEncodingException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				emission.setData(ebuf,0,ebuf.length);
				emission.setPort(portSolipsisSend); //portSolipsis + 1
				emission.setAddress(solipsisHost);
				try {
					socket1.send(emission);
					System.out.println ("send reply: " + new String(ebuf, "US-ASCII"));
					//Thread.sleep(100000);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				if (lockf.exists()== false){
					System.out.println ("state=FINISH");
					state = FINISH;
				}
			}
		}
		rtmpClient.disconnect();
		System.out.println("Finished RUN ");
	}

	public static void main( String argv[]) throws LineUnavailableException{
		MetaverseRtmpClient mc = new MetaverseRtmpClient();
		try {
			
			if (argv.length != 4){
				System.out.println("Usage: MetaverseRtmpClient mediaserver appname streamname");
				return;
			}
			// Parse argv to get MediaServer, AppName, StreamName and Home
			System.out.println("mediaserver=" + argv[0] + " appname=" + argv[1] + " streamname=" + argv [2] + " Home= " + argv[3]);
			mc.host = argv[0].toString();
			mc.app = argv[1].toString();
			mc.publishName = argv[2].toString();
			mc.metaverse_home = argv[3].toString();
			
			if ( ! mc.initNetworkAndContainer()){
				System.out.println ("init error");
				return;
			}
						
			mc.runVideoAndPush();
		}
		catch (InterruptedException e) {
			e.printStackTrace();
		} 
	}
	
	private  boolean initNetworkAndContainer() {
		
		try {
			solipsisHost = InetAddress.getByName("127.0.0.1"); // gerard 172.25.71.136
			
		} catch (UnknownHostException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		try {
			socket = new DatagramSocket(portSolipsisRecv);
		}
		catch (IOException e){
			System.out.println("datagramm socket error");
			return false;
		}
		try {
			socket1 = new DatagramSocket(); //portSolipsisSend
		}
		catch (IOException e){
			System.out.println("datagramm socket1 error");
			return false;
		}

		reception = new DatagramPacket(buf, buf.length);
		emission = new DatagramPacket(ebuf, ebuf.length);

		// Create and open the output container

		myHandler = new MyProtocolHandler();
		if (! setOutputContainer()){
			System.out.println("setOutputContainer error");
			return false;
		}

		outStreamCoder = getEncoder(outWidth, outHeight);
		if (outStreamCoder == null) {
			System.out.println ("getEncoder error");
			return false;
		}
		
		int cwhRetCode = outputContainer.writeHeader();
		if (cwhRetCode != 0) {
			final IError err = IError.make(cwhRetCode);
			System.out.println("IContainer.writeHeader() returned an error: "
							+ err.getType()
							+ ", "
							+ err.getDescription());
			return false;
		}
		
		return true;
	}
	
	private boolean setOutputContainer() {

		// Create and configure the container format object
		final IContainerFormat transcodedContainerFormat = IContainerFormat.make();
		final int cfRetCode = transcodedContainerFormat.setOutputFormat(
				containerFormatShortName, null, null);
		if (cfRetCode != 0) {
			final IError err = IError.make(cfRetCode);
			System.out
					.println("IContainerFormat.setOutputFormat() returned an error: "
							+ err.getType() + ", " + err.getDescription());
			return false;
		}
		
		System.out.println ("Successfully set output format on IContainerFormat = "
						+ transcodedContainerFormat.getOutputFormatLongName());

		// Create and open the output container
		outputContainer = IContainer.make();

		final int cRetCode = outputContainer.open(myHandler,
				IContainer.Type.WRITE, transcodedContainerFormat, true, false);

		if (cRetCode != 0) {
			final IError err = IError.make(cRetCode);
			System.out.println("IContainer.open() returned an error: "
					+ err.getType() + ", " + err.getDescription());
			return false;
		}
		
		outputContainer.addNewStream(0);

		encodedPacket = IPacket.make();
		
		System.out.println ("Successfully opened output IContainer" + " rate " );

		return true;
	}

	public static BufferedImage convertToType(BufferedImage sourceImage, 
			int targetType)
	{
		BufferedImage image;
		// if the source image is already the target type, return the source image
		if (sourceImage.getType() == targetType)
			image = sourceImage;
		// otherwise create a new image of the target type and draw the new
		// image 
		else{
			image = new BufferedImage(sourceImage.getWidth(), sourceImage.getHeight(),
					targetType);
			image.getGraphics().drawImage(sourceImage, 0, 0, null);
		}
		return image;
	}
	
	private IStreamCoder getEncoder(final int width, final int height) {

		final IStreamCoder encoder = outputContainer.getStream(0)
				.getStreamCoder();
		
		encoder.setCodec(ICodec.ID.CODEC_ID_FLV1);
		encoder.setNumPicturesInGroupOfPictures(10);
		encoder.setTimeBase(IRational.make(1, 1000));
		encoder.setPixelType(IPixelFormat.Type.YUV420P);
		encoder.setWidth(width);
		encoder.setHeight(height);
		//encoder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, true); 
		//encoder.setGlobalQuality(0); 
		final int encOpenRetVal = encoder.open();
		if (encOpenRetVal != 0) {
			final IError err = IError.make(encOpenRetVal);
			return null;
		}
		return encoder;
	}

	private void displayByteBuffer(String msg, ByteBuffer bytebuf){
		System.out.print("== " + msg + " ");

		for (int i=0 ; i < bytebuf.limit(); i++){
			System.out.print(bytebuf.get()+ " ");
		}
		System.out.println("length: " + bytebuf.limit());
	}
	
	public  void displayByteBuffer( ByteBuffer buf) {

		buf.rewind();
		while (buf.hasRemaining())
			System.out.println(buf.position() + " -> " + buf.get());	
	}


	private void displayBytes(String msg, byte[] bytes){
		System.out.print("== " + msg + " ");
		for (int i=0 ; i < bytes.length; i++){
			System.out.print(bytes[i]+ " ");
		}
		System.out.println("length: " + bytes.length);
	}


	
}


