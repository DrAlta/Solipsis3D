

import org.red5.logging.Red5LoggerFactory;
import org.slf4j.Logger;

//import com.sun.istack.internal.Nullable;
import com.xuggle.xuggler.io.IURLProtocolHandler;
import com.xuggle.xuggler.io.IURLProtocolHandlerFactory;
import com.xuggle.xuggler.io.URLProtocolManager;

/**
 * This class implements an URL protocol handler.
 * <p>
 * This allows you to obtain complete packets with file headers from a writing
 * {@link com.xuggle.xuggler.IContainer IContainer} without actually using a
 * file.
 * </p>
 * 
 * @author Nicolas Canceill
 */
public class MyProtocolHandler implements IURLProtocolHandler,
		IURLProtocolHandlerFactory {

	// ---------- Attributes. ----------

	/**
	 * This handler's log.
	 */
	private final Logger log = Red5LoggerFactory.getLogger(this.getClass(),
			"sip");

	/**
	 * The protocol handled by this handler.
	 * <p>
	 * FF-MPEG is very picky: protocol names must be only alpha characters (no
	 * number).
	 * </p>
	 */
	public final static String PROTOCOL = "myflv";

	/**
	 * The last packet written using this handler.
	 */
	private byte[] lastPacket = null;

	/**
	 * The current position in the container.
	 * <p>
	 * Used only for consistency with FF-MPEG.
	 * </p>
	 */
	private long temp = 0;

	// ---------- Constructor. ----------

	/**
	 * Creates a new MyProtocolHandler.
	 * <p>
	 * The handler is created with an empty {@link MyProtocolHandler#lastPacket
	 * lastPacket} and registered as its own factory.
	 * </p>
	 */
	public MyProtocolHandler() {
		URLProtocolManager.getManager().registerFactory(
				MyProtocolHandler.PROTOCOL, this);
		this.lastPacket = null;
	}

	// ---------- Implemented protocol factory method. ----------

	/**
	 * Called by FF-MPEG in order to get a handler to use for a given file.
	 * 
	 * @param protocol
	 *            the protocol without a ':'. For example, "file" or "http".
	 *            Unused, <i>CAN</i> be <code>null</code>.
	 * @param url
	 *            the URL that FF-MPEG is trying to open. Unused, <i>CAN</i> be
	 *            <code>null</code>.
	 * @param flags
	 *            the flags that FF-MPEG will pass to
	 *            {@link IURLProtocolHandler#open(String, int) open(String,
	 *            int)}.
	 * @return this handler.
	 */
	@Override
	public IURLProtocolHandler getHandler( final String protocol,
			 final String url, final int flags) {
		return this;
	}

	// ---------- Implemented protocol-handling methods. ----------

	/**
	 * Closes this handler by destroying its
	 * {@link MyProtocolHandler#lastPacket last packet buffer}.
	 * 
	 * @return 0 (no problem encountered).
	 */
	@Override
	public synchronized int close() {
		this.lastPacket = null;
		this.log.info("Handler closed, last packet buffer destroyed.");
		return 0;
	}

	/**
	 * Whether this handler is streamed.
	 * 
	 * @param url
	 *            the URL that would be passed to
	 *            {@link IURLProtocolHandler#open(String, int) open(String,
	 *            int)}. Unused, <i>CAN</i> be <code>null</code>.
	 * @param flags
	 *            the flags that would be passed to
	 *            {@link IURLProtocolHandler#open(String, int) open(String,
	 *            int)}.
	 * @return <code>false</code> (backwards seeking available).
	 */
	@Override
	public boolean isStreamed( final String url, final int flags) {
		return false;
	}

	/**
	 * Opens this handler by renewing its {@link MyProtocolHandler#lastPacket
	 * lastPacket}.
	 * 
	 * @param url
	 *            the URL to open. Unused, <i>CAN</i> be <code>null</code>.
	 * @param flags
	 *            the flags. <i>SHOULD</i> contain
	 *            {@link IURLProtocolHandler#URL_WRONLY_MODE URL_WRONLY_MODE}.
	 *            <i>MUST NOT</i> contain either
	 *            {@link IURLProtocolHandler#URL_RDWR URL_RDWR} or
	 *            {@link IURLProtocolHandler#URL_RDONLY_MODE URL_RDONLY_MODE}.
	 * @return 0 (no problem encountered) if the flags are valid, -1 (error)
	 *         otherwise.
	 */
	@Override
	public int open( final String url, final int flags) {
		switch (flags) {
		case IURLProtocolHandler.URL_WRONLY_MODE:
			synchronized (this) {
				this.lastPacket = null;
			}
			this.log.info("Handler opened in write-only mode, "
					+ "last packet buffer renewed.");
			return 0;
		case IURLProtocolHandler.URL_RDONLY_MODE:
		case IURLProtocolHandler.URL_RDWR:
			this.log.error("Could not open handler: "
					+ "use write-only mode, others are unsupported.");
			return -1;
		default:
			this.log.warn("Invalid flags passed to open(String, int), ignoring flags: "
					+ "mode set to write-only.");
			synchronized (this) {
				this.lastPacket = null;
			}
			this.log.info("Handler opened in write-only mode, "
					+ "last packet buffer renewed.");
			return 0;
		}
	}

	/**
	 * Does nothing (an FLV protocol handler is not made for reading).
	 * 
	 * @param buf
	 *            the buffer to write data to. Unused, <i>CAN</i> be
	 *            <code>null</code>.
	 * @param size
	 *            the number of bytes of data available in the given buffer.
	 * @return 0 (end of file).
	 */
	@Override
	public int read( final byte[] buf, final int size) {
		this.log.error("Reading unsupported!");
		return 0;
	}

	/**
	 * Does nothing (an FLV protocol handler is not made for seeking).
	 * 
	 * @param offset
	 *            the offset in bytes.
	 * @param whence
	 *            where that offset is relative to. Follow the C stdlib fseek()
	 *            conventions, except that {@link IURLProtocolHandler#SEEK_SIZE
	 *            SEEK_SIZE} should return back the size of the stream in bytes
	 *            if known without adjusting the seek pointer.
	 * @return an arbitrary high value (65535) if the
	 *         {@link IURLProtocolHandler#SEEK_SIZE SEEK_SIZE} flag is passed
	 *         in, the passed-in offset if the
	 *         {@link IURLProtocolHandler#SEEK_SET SEEK_SET} flag is passed in,
	 *         the passed-in offset plus the {@link MyProtocolHandler#temp temp}
	 *         value if the {@link IURLProtocolHandler#SEEK_CUR SEEK_CUR} flag
	 *         is passed in, the passed-in offset plus an arbitrary high value
	 *         (65535) if the {@link IURLProtocolHandler#SEEK_END SEEK_END} flag
	 *         is passed in, and -1 otherwise.
	 */
	@Override
	public long seek(final long offset, final int whence) {
		final int length = 65535;
		switch (whence) {
		case IURLProtocolHandler.SEEK_SIZE:
			return length;
		case IURLProtocolHandler.SEEK_SET:
			this.temp = offset;
			return this.temp;
		case IURLProtocolHandler.SEEK_CUR:
			this.temp += offset;
			return this.temp;
		case IURLProtocolHandler.SEEK_END:
			this.temp = length + offset;
			return this.temp;
		default:
			this.log.error("Invalid passed-in seeking flag.");
			return -1;
		}
	}

	/**
	 * Saves the given byte array as the {@link MyProtocolHandler#lastPacket
	 * lastPacket}.
	 * 
	 * @param buf
	 *            the buffer to write data from.
	 * @param size
	 *            the number of bytes to read in the given buffer.
	 * @return the number of bytes written, or -1 (error).
	 */
	@Override
	public synchronized int write(final byte[] buf, final int size) {
		this.lastPacket = null;
		if (size > 0 && buf != null && buf.length >= size) {
			/*
			 * this.lastPacket = new byte[size]; System .arraycopy(buf, 0,
			 * this.lastPacket, 0, size); log.debug ("Writing " + size +
			 * " bytes: " + MyProtocolHandler . printBytes(buf));
			 */
			this.lastPacket = buf;
			return size;
		}
		this.log.error("Could not write: invalid argument(s).");
		return -1;
	}

	// ---------- External method. ----------

	/**
	 * Gets the last packet written using this handler.
	 * 
	 * @return the last packet.
	 */
	public synchronized byte[] getLastPacket() {
		return this.lastPacket;
	}

}
