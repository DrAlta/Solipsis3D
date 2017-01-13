

import java.nio.ByteBuffer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Util {
	
	
	/** The logger */
	protected static Logger log = null;
	
	protected static Logger logger = LoggerFactory.getLogger(Util.class);
	

	protected static void display20Hexa(byte[] bytes, String paramName,int seqNumber, Class clazz, boolean isDebug) {
		String str = "";
		log = LoggerFactory.getLogger(clazz);
		if (bytes == null){
			str = "null !!!";
			log.debug(str);
			return;
		}		
		str = "payload L=" + bytes.length + " ("+paramName+"="+seqNumber+")= {";
		if (bytes.length < 60){
			for (int i = 0; i < bytes.length; i++) {
				if (i == bytes.length - 1)
					str += Integer.toHexString((int)(bytes[i]& 0xFF)) + "}";
				else
					str += Integer.toHexString((int)(bytes[i]& 0xFF)) + ",";
			}
			log.debug(str);
			return;
		}	
		int length = 30;
		for (int i = 0; i < length; i++) {
			if (i == length - 1)
				str += Integer.toHexString((int)(bytes[i]& 0xFF)) + ",...,";
			else
				str += Integer.toHexString((int)(bytes[i]& 0xFF)) + ",";
		}
		for (int i = bytes.length-20; i < bytes.length; i++) {
			if (i == bytes.length - 1)
				str += Integer.toHexString((int)(bytes[i]& 0xFF)) + "}";
			else
				str += Integer.toHexString((int)(bytes[i]& 0xFF)) + ",";
		}		
		if (isDebug){
			log.debug(str);
		}else{
			log.info(str);
		}
	}
	
	protected static void display20Bytes(byte[] bytes,String paramName, int seqNumber, Class clazz, boolean isDebug) {
		String str = "";
		log = LoggerFactory.getLogger(clazz);
		if (bytes == null){
			str = "null !!!";
			log.debug(str);
			return;
		}
		str = "payload L=" + bytes.length + " ("+paramName+"="+seqNumber+")= {";
		if (bytes.length < 60){
			for (int i = 0; i < bytes.length; i++) {
				if (i == bytes.length - 1)
					str += bytes[i] + "}";
				else
					str += bytes[i] + ",";
			}
			log.debug(str);
			return;
		}
		int length = 30;	
		for (int i = 0; i < length; i++) {
			if (i == length - 1)
				str += bytes[i] + ",...,";
			else
				str += bytes[i] + ",";
		}
		for (int i = bytes.length-10; i < bytes.length; i++) {
			if (i == bytes.length - 1)
				str += bytes[i] + "}";
			else
				str += bytes[i] + ",";
		}
		if (isDebug){
			log.debug(str);
		}else{
			log.info(str);
		}
		
	}
	
	public static Integer getSequenceNumber(byte b1,byte b2){
		Short value = null;
		Integer out = null;
		ByteBuffer buff = ByteBuffer.allocate(2);
		buff.put(b1);
		buff.put(b2);
		value = buff.getShort(0);
		String str2 = " packet[2] " + b1 + " Hexa =" + Integer.toHexString((int)(b1& 0xFF));
		String str3 = " packet[3] " + b2 + " Hexa =" + Integer.toHexString((int)(b2& 0xFF));
		if (value !=null){
			logger.debug("Sequence number: " + value + str2 + str3 );
			out = (int) value + (int)Short.MAX_VALUE;
		}		 
		return out;
	}
	
	public static Long getTimestamp(byte[] packet){
		Integer ts = null;
		ByteBuffer buff = ByteBuffer.allocate(4);
		buff.put(packet, 4, 4);
		ts = buff.getInt(0);
		String tsHexa = Integer.toHexString((int)(packet[4]& 0xFF))+ " ";
		tsHexa += Integer.toHexString((int)(packet[5]& 0xFF))+ " ";
		tsHexa += Integer.toHexString((int)(packet[6]& 0xFF))+ " ";
		tsHexa += Integer.toHexString((int)(packet[7]& 0xFF));
		Long timestamp = null;
		if(ts != null){
			logger.debug("timestamp (int): " + ts + " " + tsHexa);
			timestamp = (long) ts + (long) Integer.MAX_VALUE;
			//logger.debug("timestamp (long): " + timestamp);
		}
		return timestamp;
	}
	
	public static void displayByte(byte[] bytes,String intro ,Class clazz) {
		String str = intro;
		log = LoggerFactory.getLogger(clazz);
		for (int i = 0; i < bytes.length; i++) {
			if (i == 0) {
				str += " (L= " + bytes.length + " )= {"+ bytes[i] + ",";
			} else if (i == bytes.length - 1)
				str += bytes[i] + "}";
			else
				str += bytes[i] + ",";
		}
		log.debug(str);
	}
	
	public static void displayByteHexa(byte[] bytes,String intro ,Class clazz) {
		String str = intro;
		log = LoggerFactory.getLogger(clazz);
		String value = "";
		for (int i = 0; i < bytes.length; i++) {
			value = Integer.toHexString((int)(bytes[i]& 0xFF));
			if (i == 0) {
				str += " (L= " + bytes.length + " )= {"+ value + ",";
			} else if (i == bytes.length - 1)
				str += value + "}";
			else
				str += value + ",";
		}
		log.debug(str);
	}
	
	
	public static byte[] intToByteArray(int value) {
		byte[] b = new byte[4];
		for (int i = 0; i < 4; i++) {
			int offset = (b.length - 1 - i) * 8;
			b[i] = (byte) ((value >>> offset) & 0xFF);
		}
		return b;
	}
	
	
	
	  /**
	   * Converts a portion of a byte array with given offset to short value.
	   *
	   * @param paRawBytes the byte array
	   * @param piOffset offset in the original array to start reading bytes from
	   * @param pbBigEndian true if the bytes are in Big-endian order; false otherwise
	   *
	   * @return short representation of the bytes
	   */
	public static short byteArrayToShort(byte[] paRawBytes, int piOffset,
			boolean pbBigEndian) {
		int iRetVal = -1;

		// TODO: revisit this: should we silently add missing byte and should
		// we ingnore excessing bytes?
		if (paRawBytes.length < piOffset + 2)
			return -1;

		int iLow;
		int iHigh;

		if (pbBigEndian) {
			iLow = paRawBytes[piOffset + 1];
			iHigh = paRawBytes[piOffset + 0];
		} else {
			iLow = paRawBytes[piOffset + 0];
			iHigh = paRawBytes[piOffset + 1];
		}

		// Merge high-order and low-order byte to form a 16-bit double value.
		iRetVal = (iHigh << 8) | (0xFF & iLow);

		return (short) iRetVal;
	}
	
	
	  /**
	   * Converts a portion of a byte array with given offset to int value.
	   *
	   * @param paRawBytes the byte array
	   * @param piOffset offset in the original array to start reading bytes from
	   * @param pbBigEndian true if the bytes are in Big-endian order; false otherwise
	   *
	   * @return int representation of the bytes
	   */
	
	public static int byteArrayToInt(byte[] paRawBytes, int piOffset,
			boolean pbBigEndian) {
		int iRetVal = -1;

		if (paRawBytes.length < piOffset + 4)
			return iRetVal;

		int iLowest;
		int iLow;
		int iMid;
		int iHigh;

		if (pbBigEndian) {
			iLowest = paRawBytes[piOffset + 3];
			iLow = paRawBytes[piOffset + 2];
			iMid = paRawBytes[piOffset + 1];
			iHigh = paRawBytes[piOffset + 0];
		} else {
			iLowest = paRawBytes[piOffset + 0];
			iLow = paRawBytes[piOffset + 1];
			iMid = paRawBytes[piOffset + 2];
			iHigh = paRawBytes[piOffset + 3];
		}

		// Merge four bytes to form a 32-bit int value.
		iRetVal = (iHigh << 24) | (iMid << 16) | (iLow << 8) | (0xFF & iLowest);

		return iRetVal;
	}
	
}
