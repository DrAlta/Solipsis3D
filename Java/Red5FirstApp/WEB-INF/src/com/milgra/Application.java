package com.milgra;

import java.sql.Timestamp;
import java.util.Date;
import java.util.Properties;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

import org.red5.server.adapter.ApplicationAdapter;
import org.red5.server.api.IClient;
import org.red5.server.api.IConnection;
import org.red5.server.api.IScope;
import org.red5.server.api.Red5;
import org.red5.server.api.service.IServiceCapableConnection;
import org.red5.server.api.stream.IBroadcastStream;

import java.io.FileInputStream;
import java.io.IOException;


public class Application extends ApplicationAdapter {

	/**
	 * Class logger.
	 */
	private static final Logger logger = Logger.getLogger(Application.class.getName());
	
	/**
	 * Properties filename.
	 */
	private static final String PROPSFILENAME = "webapps/vhc/WEB-INF/VHC.properties";
	
	/**
	 * Logs filename.
	 */
	private static final String LOGSFILENAME = "webapps/vhc/WEB-INF/logs/VHCTrace-";
	
	/**
	 * VHC properties.
	 */
	public static Properties properties;
	
	private IServiceCapableConnection vhcCtrl = null;
	
	/* (non-Javadoc)
	 * @see org.red5.server.adapter.MultiThreadedApplicationAdapter#appStart(org.red5.server.api.IScope)
	 */
	@Override
	public boolean appStart(IScope scope) {
		initLogger();
		initProperties();
		logger.info("Application Virtual Hybrid Communication started.");
	
		return super.appStart(scope);
	}
	
	/**
	 * Set global logger.
	 */
	private void initLogger() {
		String filename = LOGSFILENAME + new Timestamp(new Date().getTime());

		Logger.getLogger("com.milgra").setLevel(Level.ALL);
		try {
			FileHandler fh = new FileHandler(filename.replaceAll("\\s", "T")
					.replaceFirst(":", "h")
					.replaceFirst(":", "m")
					.replaceFirst("\\.", "s"));
			fh.setLevel(Level.ALL);
			fh.setFormatter(new SimpleFormatter());
			Logger.getLogger("com.milgra").addHandler(fh);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Load application's property file
	 */
	private void initProperties() {
	    properties = new Properties();
	    
	    try {
	        properties.load(new FileInputStream(PROPSFILENAME));
	    } catch (IOException e) {
			System.out.println("Error, cannot read prop file.");
	    }

	}
	
	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#connect(org.red5.server.api.IConnection, org.red5.server.api.IScope, java.lang.Object[])
	 */
	@Override
	public synchronized boolean connect(
			IConnection conn,
			IScope scope,
			Object[] params) {

		logger.info("New connection from: " + Red5.getConnectionLocal().getRemoteAddress());
		
		// Only one (optional) parameter accepted: the mode. 
		if (params.length > 0)
			if (params[0].equals("vhcCtrl")) {
				vhcCtrl = (IServiceCapableConnection) conn;
				conn.getClient().setAttribute("mode", "vhcCtrl");
				logger.info("Registered new vhcCtrl.");
			} else {
				conn.getClient().setAttribute("mode", "client");
				logger.info("Registered new client");
			}
		
		return super.connect(conn, scope, params);
	}

	/**
	 * Method called by the client to switch streams.
	 * @return "switched" on success, "failed" on... failed
	 */
	public String setStream(int idx) {
		logger.info("Request for stream #" + idx);

		if (vhcCtrl != null){
			logger.info("  request invoque");
			vhcCtrl.invoke("setStream", new Object[]{idx});
		}
	    return "switched";
	}
	
	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#disconnect(org.red5.server.api.IConnection, org.red5.server.api.IScope)
	 */
	@Override
	public synchronized void disconnect(IConnection conn, IScope scope) {
		super.disconnect(conn, scope);
	}

	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#join(org.red5.server.api.IClient, org.red5.server.api.IScope)
	 */
	@Override
	public synchronized boolean join(IClient client, IScope scope) {
		
		return super.join(client, scope);
	}

	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#leave(org.red5.server.api.IClient, org.red5.server.api.IScope)
	 */
	@Override
	public synchronized void leave(IClient client, IScope scope) {
		
		if ((vhcCtrl != null) && (client.getAttribute("mode").equals("vhcCtrl"))) {
				logger.info("vhcCtrl unregistered");
				vhcCtrl = null;
		}
		
		super.leave(client, scope);
	}

	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#start(org.red5.server.api.IScope)
	 */
	@Override
	public synchronized boolean start(IScope scope) {
		return super.start(scope);
	}

	/* (non-Javadoc)
	 * @see org.red5.server.adapter.ApplicationAdapter#stop(org.red5.server.api.IScope)
	 */
	@Override
	public synchronized void stop(IScope scope) {
		super.stop(scope);
	}


	/* (non-Javadoc)
	 * @see org.red5.server.adapter.MultiThreadedApplicationAdapter#streamPublishStart(org.red5.server.api.stream.IBroadcastStream)
	 */
	@Override
	public void streamPublishStart(IBroadcastStream stream) {

		logger.info("New incoming stream from: " + Red5.getConnectionLocal().getRemoteAddress());
		
		super.streamPublishStart(stream);
	}
	
	
}
