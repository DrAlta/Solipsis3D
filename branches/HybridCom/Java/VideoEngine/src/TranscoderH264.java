

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;

import com.xuggle.xuggler.Configuration;
import com.xuggle.xuggler.ICodec;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;
import com.xuggle.xuggler.IError;
import com.xuggle.xuggler.IPacket;
import com.xuggle.xuggler.IPixelFormat;
import com.xuggle.xuggler.IRational;
import com.xuggle.xuggler.IStream;
import com.xuggle.xuggler.IStreamCoder;
import com.xuggle.xuggler.IVideoPicture;
import com.xuggle.xuggler.IVideoResampler;
import com.xuggle.xuggler.ICodec.ID;

public class TranscoderH264 {
    // Initial settings specific to desired output format
    static String containerFormatShortName = "flv";
    static IPixelFormat.Type supportedOutputPixelType = IPixelFormat.Type.YUV420P;

    static IContainer inputContainer;
    static IStreamCoder inputVideoCoder;
    static IStream inputStream;
    static IVideoResampler inputResampler;
    static int inputVideoStreamId;

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {

        setupInput();

        // Create and configure a resampler to convert pictures from one
        // pixel format to another
        IVideoResampler videoResampler = IVideoResampler.make(inputResampler.getOutputWidth(),
                inputResampler.getOutputHeight(), supportedOutputPixelType,
                inputResampler.getOutputWidth(), inputResampler.getOutputHeight(),
                inputResampler.getOutputPixelFormat());

        // Create a file output stream
        RandomAccessFile transcodedFileStream = new RandomAccessFile("transcoded."
                + containerFormatShortName, "rw");

        // Create and configure the container format object
        IContainerFormat transcodedContainerFormat = IContainerFormat.make();
        int cfRetCode = transcodedContainerFormat.setOutputFormat(containerFormatShortName, null, null);
        if (cfRetCode != 0) {
            IError err = IError.make(cfRetCode);
            System.out.println("IContainerFormat.setOutputFormat() returned an error: "
                    + err.getType() + ", " + err.getDescription());
            return;
        }
        System.out.println("Successfully set output format on IContainerFormat");

     // Create and open the output container
        IContainer writeContainer = IContainer.make();
        // open RTMP stream for publishing
        int rmptCode = writeContainer.open("rtmp://localhost/oflaDemo/test", IContainer.Type.WRITE, transcodedContainerFormat,
        		true, false);
        if (rmptCode != 0){
            IError err = IError.make(rmptCode);
            System.out.println("IContainer.open() returned an error: "
                    + err.getType() + ", " + err.getDescription());
            return;
        }
        System.out.println("Successfully opened output IContainer (rtmp)");
        writeContainer.addNewStream(0);
        
        
//        // Create and open the output container
//        IContainer transcodingContainer = IContainer.make();
//        int cRetCode = transcodingContainer.open(transcodedFileStream, IContainer.Type.WRITE,
//            transcodedContainerFormat,
//                true, false);
//        if (cRetCode != 0) {
//            IError err = IError.make(cRetCode);
//            System.out.println("IContainer.open() returned an error: "
//                    + err.getType() + ", " + err.getDescription());
//            return;
//        }
//        System.out.println("Successfully opened output IContainer");
//        transcodingContainer.addNewStream(0);


        // Create a packet to serve as a destination for the encoder's output
        IPacket encodedPacket = IPacket.make();

        // Create and configure the encoder
        
        // TODO modif
        //IStreamCoder encoder = transcodingContainer.getStream(0).getStreamCoder();
        IStreamCoder encoder = writeContainer.getStream(0).getStreamCoder();
        
        encoder.setCodecID(ICodec.ID.CODEC_ID_H264);
        int retval =
        	   Configuration.configure("D:/Tools/Xuggle/share/ffmpeg/libx264-default.ffpreset",
        			   encoder);
        	 if (retval < 0)
        	   throw new RuntimeException("could not configure coder from preset file");
//        ICodec theCodec = encoder.getCodec();
//        encoder.setTimeBase(inputVideoCoder.getTimeBase());
        encoder.setTimeBase(IRational.make(1,25));
        encoder.setPixelType(supportedOutputPixelType);
        encoder.setWidth(videoResampler.getOutputWidth());
        encoder.setHeight(videoResampler.getOutputHeight());
        encoder.setChannels(1);
        encoder.setFlag(IStreamCoder.Flags.FLAG_GLOBAL_HEADER, true);
        encoder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, true);
        encoder.setGlobalQuality(4);
        //encoder.setProperty("qscale", 4);
        //encoder.setSampleRate(22050);
        //encoder.setBitRate(64000);
        encoder.setAutomaticallyStampPacketsForStream(true);
        int encOpenRetVal = encoder.open();
        if (encOpenRetVal != 0) {
            IError err = IError.make(encOpenRetVal);
            System.out.println("IStreamCoder.open() returned an error: "
                    + err.getType() + ", " + err.getDescription());
            return;
        }
        
        // TODO modif
        //int cwhRetCode = transcodingContainer.writeHeader();
        int cwhRetCode = writeContainer.writeHeader();
        if (cwhRetCode != 0) {
            IError err = IError.make(cwhRetCode);
            System.out.println("IContainer.writeHeader() returned an error: "
                    + err.getType() + ", " + err.getDescription());
            return;
        }
        System.out.println("Successfully wrote header to IContainer");

        IVideoPicture originalInputPic = null;

        // Create a video picture to serve as a destination for the inputResampler's
        // output
        IVideoPicture resampledOutputPic = IVideoPicture.make(
                videoResampler.getOutputPixelFormat(),
                videoResampler.getOutputWidth(),
                videoResampler.getOutputHeight());

        // Transcode input to output
        while (true) {
            // Read picture from input
            try {
                originalInputPic = readNextVideoPicture();
                //System.out.println("Successfully got a video picture from the input source");
            } catch (EOFException eofex) {
                break;
            }

            // Resample picture
            videoResampler.resample(resampledOutputPic, originalInputPic);

            // Encode resampled picture to a packet
            int encodeVideoRetCode = encoder.encodeVideo(encodedPacket, resampledOutputPic, -1);
            if (encodeVideoRetCode < 0) {
                IError err = IError.make(encodeVideoRetCode);
                System.out.println("IStreamCoder.encodeVideo() returned an error: "
                        + err.getType() + ", " + err.getDescription());
            }

            // Write encoded packet to output inputContainer
            if (encodedPacket.isComplete()) {
            	 // TODO modif
                //int writePacketRetCode = transcodingContainer.writePacket(encodedPacket);
            	int writePacketRetCode = writeContainer.writePacket(encodedPacket);   	
                if (writePacketRetCode != 0) {
                    IError err = IError.make(writePacketRetCode);
                    System.out.println("IContainer.writePacket() returned an error: " + err.getType() + ", " + err.getDescription());
                }
                System.out.println("Successfully write picture into output container");
            }
        }

        int flushEncoderRetCode;
        do {
            flushEncoderRetCode = encoder.encodeVideo(encodedPacket, null, -1);
            System.out.println("Flushed a packet through the video encoder, which returned " + flushEncoderRetCode);
            if (encodedPacket.isComplete()) {
                System.out.println("The flushed packet is complete");
            } else {
                System.out.println("The flushed packet is incomplete");
            }
        } while ((flushEncoderRetCode >= 0) && encodedPacket.isComplete());
        
        // TODO modif
        //int cwtRetCode = transcodingContainer.writeTrailer();
        int cwtRetCode = writeContainer.writeTrailer();
        if (cwtRetCode != 0) {
            IError err = IError.make(cwtRetCode);
            System.out.println("IContainer.writeHeader() returned an error: "
                    + err.getType() + ", " + err.getDescription());
            return;
        }

        encoder.close();
        encoder = null;
        
        // TODO modif
//        transcodingContainer.close();
//        transcodingContainer = null;
        writeContainer.close();
        writeContainer = null;
    }

    private static void setupInput() {
    	
        // Let's make sure that we can actually convert video pixel formats.
        if (!IVideoResampler.isSupported(
                IVideoResampler.Feature.FEATURE_COLORSPACECONVERSION)) {
            throw new RuntimeException("you must install the GPL version" +
                    " of Xuggler (with IVideoResampler support) for " +
                    "this demo to work");
        }

        // Create a Xuggler inputContainer object
        inputContainer = IContainer.make();

        // Open up the inputContainer
//        if (inputContainer.open(filename, IContainer.Type.READ, null) < 0) {
//            throw new IllegalArgumentException("could not open file: " + filename);
//        }
        
		IContainerFormat inputContainerFormat = IContainerFormat.make();
		inputContainerFormat.setInputFormat("sdp");
		String sdpPath = "data/sdpdescriptors/h263sdp.sdp";
		File sdpFile = new File(sdpPath);

		String path;
		try {
			path = sdpFile.getCanonicalPath();
			if (inputContainer.open(path, IContainer.Type.READ,
					inputContainerFormat, true, false) < 0) {
				throw new RuntimeException("Xuggler container.open failed! "
						+ sdpFile.getCanonicalPath());
			}
		} catch (IOException e) {
			e.printStackTrace();
		}  

        // query how many streams the call to open found
        int numStreams = inputContainer.getNumStreams();

        // and iterate through the streams to find the first video inputStream
        inputVideoStreamId = -1;
        inputVideoCoder = null;
        for (int i = 0; i < numStreams; i++) {
            // Find the inputStream object
            inputStream = inputContainer.getStream(i);
            // Get the pre-configured decoder that can decode this inputStream;
            IStreamCoder coder = inputStream.getStreamCoder();

            if (coder.getCodecType() == ICodec.Type.CODEC_TYPE_VIDEO) {
                inputVideoStreamId = i;
                inputVideoCoder = coder;
                break;
            }
        }
        if (inputVideoStreamId == -1) {
            throw new RuntimeException("could not find video stream in container: " + sdpPath);
        }

        /*
         * Now we have found the video inputStream in this file.  Let's open up our decoder so it can
         * do work.
         */
        if (inputVideoCoder.open() < 0) {
            throw new RuntimeException("could not open video decoder for container: " + sdpPath);
        }

        inputResampler = null;
        if (inputVideoCoder.getPixelType() != IPixelFormat.Type.BGR24) {
            // if this inputStream is not in BGR24, we're going to need to
            // convert it.  The VideoResampler does that for us.
        	int inputWidth = inputVideoCoder.getWidth();
        	int inputHeight = inputVideoCoder.getHeight();
        	if (inputWidth <= 0){
        		inputWidth = 352;
        	}
        	if (inputHeight <= 0){
        		inputHeight = 288;
        	}
        	int outputWidth = 352;
        	int outputHeight = 288;
            inputResampler = IVideoResampler.make(outputWidth,
            		outputHeight, IPixelFormat.Type.BGR24,
            		inputWidth, inputHeight, inputVideoCoder.getPixelType());
            if (inputResampler == null) {
                throw new RuntimeException("could not create color space " +
                        "resampler for: " + sdpPath);
            }
        }

    }

    static IVideoPicture readNextVideoPicture() throws EOFException {
        /*
         * Now, we start walking through the container looking at each packet.
         */
        IPacket packet = IPacket.make();
        IVideoPicture newPic = null;

        do {
            int readReturnVal = inputContainer.readNextPacket(packet);
            if (readReturnVal < 0) {
                throw new EOFException();
            }
        } while (packet.getStreamIndex() != inputVideoStreamId);


        /*
         * We allocate a new picture to get the data out of Xuggler
         */
        IVideoPicture picture = IVideoPicture.make(inputVideoCoder.getPixelType(),
                inputVideoCoder.getWidth(), inputVideoCoder.getHeight());

        int offset = 0;
        while (offset < packet.getSize()) {
            /*
             * Now, we decode the video, checking for any errors.
             *
             */
            int bytesDecoded = inputVideoCoder.decodeVideo(picture, packet, offset);
            if (bytesDecoded < 0) {
                throw new RuntimeException("got error decoding video");
            }
            offset += bytesDecoded;

            /*
             * Some decoders will consume data in a packet, but will not be able to construct
             * a full video picture yet.  Therefore you should always check if you
             * got a complete picture from the decoder
             */
            if (picture.isComplete()) {
                newPic = picture;
                /*
                 * If the resampler is not null, that means we didn't get the
                 * video in BGR24 format and
                 * need to convert it into BGR24 format.
                 */
                if (inputResampler != null) {
                    // we must resample
                    newPic = IVideoPicture.make(inputResampler.getOutputPixelFormat(),
                            picture.getWidth(), picture.getHeight());
                    if (inputResampler.resample(newPic, picture) < 0) {
                        throw new RuntimeException("could not resample video");
                    }
                }
                if (newPic.getPixelType() != IPixelFormat.Type.BGR24) {
                    throw new RuntimeException("could not decode video" +
                            " as BGR 24 bit data");
                }

            }
        }

        return newPic;
    }
}
