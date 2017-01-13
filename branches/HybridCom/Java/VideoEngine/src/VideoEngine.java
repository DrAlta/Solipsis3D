import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.*;
import java.net.*;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import com.xuggle.xuggler.*;
import com.xuggle.xuggler.video.ConverterFactory;
import com.xuggle.xuggler.video.IConverter;
import com.sun.image.codec.jpeg.*;

public class VideoEngine {
	public static void main(String argv[])
	throws SocketException, IOException
	{
		byte[] tampon = new byte[1024];
		byte[] image = new byte[1024];
		String filename;	
		BufferedImage loadImg = null;
		long firstTimeStamp = 0;
		int retval, nframe = 0;
		
		JFrame frame = new JFrame("metaverse");
		
		DatagramSocket socket =new DatagramSocket(3000);
		DatagramPacket reception = new DatagramPacket(tampon, tampon.length);
		
		IContainer outContainer = IContainer.make();
		IContainerFormat containerFormat = IContainerFormat.make();
		retval = containerFormat.setOutputFormat("flv", null, null);
		if (retval != 0){
			IError err = IError.make(retval);
			System.out.println("setOutputFormat error " + err.getType() + ", " + err.getDescription());
		}
		RandomAccessFile outf = new RandomAccessFile("c://Development/HybridCom/outFile.flv", "rw");
		retval = outContainer.open(outf, IContainer.Type.WRITE, containerFormat, false, false);
		if (retval < 0)
			throw new RuntimeException ("could not open output file");
		
		IStream outStream = outContainer.addNewStream(0);
		IStreamCoder outStreamCoder = outStream.getStreamCoder();
		ICodec codec = ICodec.guessEncodingCodec(null,null,"file://Development/HybridCom/outFile.flv",
				null, ICodec.Type.CODEC_TYPE_VIDEO);
		
		outStreamCoder.setNumPicturesInGroupOfPictures(1); 
		outStreamCoder.setCodec(codec);  
		//outStreamCoder.setBitRate(25000); 
		//outStreamCoder.setBitRateTolerance(9000); 
		outStreamCoder.setPixelType(IPixelFormat.Type.YUV420P); 
		outStreamCoder.setHeight(240); 
		outStreamCoder.setWidth(320); 
		outStreamCoder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, true); 
		outStreamCoder.setGlobalQuality(0); 

		IRational frameRate = IRational.make(3,1); 
		outStreamCoder.setFrameRate(frameRate); 
		outStreamCoder.setTimeBase(IRational.make(frameRate.getDenominator(), frameRate.getNumerator())); 
		frameRate = null; 

		outStreamCoder.open(); 
		outContainer.writeHeader(); 
		
		System.out.println("attente sur le port 3000");
		long ts =0;
		while (true) {
			socket.receive(reception);
			filename=new String(tampon, 0, reception.getLength());
			//System.out.println("Reception de la machine " + reception.getAddress().getHostName()+
			//		" sur le port " + reception.getPort() + " :\n" + texte );
			System.out.println("received " + reception.getLength() + " bytes " + "filename: " + filename);
			
			File f = new File(filename);
			try {
			  loadImg = ImageIO.read(f);
			  
			}
			catch (IOException e) {
				System.out.println("loadimg error");
			}
			nframe += 1;
			if (nframe < 200){
				f.delete();
				continue;
			}
			if (nframe > 500){
				f.delete();
				break;
			}
			// ok
			BufferedImage xugBufferedImage = convertToType(loadImg,
					BufferedImage.TYPE_3BYTE_BGR);

			IPacket packet = IPacket.make(); 
			IConverter converter = ConverterFactory.createConverter(xugBufferedImage,
					IPixelFormat.Type.YUV420P);

			long now = System.currentTimeMillis(); 
			if (firstTimeStamp == -1) 
				firstTimeStamp = now;
			long timeStamp = (now - firstTimeStamp)*1000; // convert to microseconds 
			
			IVideoPicture outFrame = converter.toPicture(xugBufferedImage, timeStamp); 
			outFrame.setQuality(0); 
			outStreamCoder.encodeVideo(packet, outFrame, 0);

			if (packet.isComplete()) {
				outContainer.writePacket(packet); 
			}
		}
		outContainer.writeTrailer();
    }
	
	/** 
	  * Convert a {@link BufferedImage} of any type, to {@link
	  * BufferedImage} of a specified type.  If the source image is the
	  * same type as the target type, then original image is returned,
	  * otherwise new image of the correct type is created and the content
	  * of the source image is copied into the new image.
	  *
	  * @param sourceImage the image to be converted
	  * @param targetType the desired BufferedImage type 
	  *
	  * @return a BufferedImage of the specifed target type.
	  *
	  * @see BufferedImage
	  */

	 public static BufferedImage convertToType(BufferedImage sourceImage, 
	   int targetType)
	 {
	   BufferedImage image;

	   // if the source image is already the target type, return the source image

	   if (sourceImage.getType() == targetType)
	     image = sourceImage;

	   // otherwise create a new image of the target type and draw the new
	   // image 

	   else
	   {
	     image = new BufferedImage(sourceImage.getWidth(), sourceImage.getHeight(),
	       targetType);
	     image.getGraphics().drawImage(sourceImage, 0, 0, null);
	   }
	    
	   return image;
	 }
}

//frame.setVisible(false);
//JLabel label = new JLabel(new ImageIcon(loadImg));
//frame.getContentPane().add(label);
//frame.pack();
//frame.setLocation(100,100);
//frame.setVisible(true);

//BufferedImage bi = new BufferedImage(loadImg.getWidth(), loadImg.getHeight(), 
//		BufferedImage.TYPE_INT_RGB);
//Graphics g = bi.getGraphics();
//frame.setBounds(0, 0, loadImg.getWidth(), loadImg.getHeight());
//frame.setVisible(true);
//g.drawImage(loadImg, 0, 0, null);

