/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * 10Feb2013 Casey Gerstle
 * 
 * Modifications to the original android example code to...
 * 		1. be part of my package
 * 		2. use my UUID for my bluetooth device
 * 		3. Send/receive messages slightly different
 */

package com.inappropirates.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.UUID;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.inappropirates.remotecontrol.AppUtil;

/**
 * This class does all the work for setting up and managing Bluetooth
 * connections with other devices. It has a thread that listens for
 * incoming connections, a thread for connecting with a device, and a
 * thread for performing data transmissions when connected.
 */
public class BluetoothChatService {
    private static final String NAME_SECURE = "LightWalkerSecure";

    // Unique UUID for this application
    private static final UUID MY_UUID_SECURE = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // Member fields
    private final BluetoothAdapter mAdapter;
    private final Handler mHandler;
    private AcceptThread mSecureAcceptThread;
    private ConnectThread mConnectThread;
    private ConnectedThread mConnectedThread;
    private int mState;

    // Constants that indicate the current connection state
    public static final int STATE_NONE = 0;       // we're doing nothing
    public static final int STATE_LISTEN = 1;     // now listening for incoming connections
    public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
    public static final int STATE_CONNECTED = 3;  // now connected to a remote device

    /**
     * Constructor. Prepares a new BluetoothChat session.
     * @param context  The UI Activity Context
     * @param handler  A Handler to send messages back to the UI Activity
     */
    public BluetoothChatService(Context context, Handler handler) {
        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mState = STATE_NONE;
        mHandler = handler;
        
    }

    /**
     * Set the current state of the chat connection
     * @param state  An integer defining the current connection state
     */
    private synchronized void setState(int state) {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "setState() " + mState + " -> " + state);
        mState = state;

        // Give the new state to the Handler so the UI Activity can update
        mHandler.obtainMessage(AppUtil.MESSAGE_STATE_CHANGE, state, -1).sendToTarget();
    }

    /**
     * Return the current connection state. */
    public synchronized int getState() {
        return mState;
    }

    /**
     * Start the chat service. Specifically start AcceptThread to begin a
     * session in listening (server) mode. Called by the Activity onResume() */
    public synchronized void start() {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "start");

        // Cancel any thread attempting to make a connection
        if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}

        // Cancel any thread currently running a connection
        if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

        setState(STATE_LISTEN);

        // Start the thread to listen on a BluetoothServerSocket
        if (mSecureAcceptThread == null) {
            mSecureAcceptThread = new AcceptThread(true);
            mSecureAcceptThread.start();
        }
    }

    /**
     * Start the ConnectThread to initiate a connection to a remote device.
     * @param device  The BluetoothDevice to connect
     */
    public synchronized void connect(BluetoothDevice device) {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "connect to: " + device);

        // Cancel any thread attempting to make a connection
        if (mState == STATE_CONNECTING) {
            if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}
        }

        // Cancel any thread currently running a connection
        if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

        // Start the thread to connect with the given device
        mConnectThread = new ConnectThread(device);
        mConnectThread.start();
        setState(STATE_CONNECTING);
    }

    /**
     * Start the ConnectedThread to begin managing a Bluetooth connection
     * @param socket  The BluetoothSocket on which the connection was made
     * @param device  The BluetoothDevice that has been connected
     */
    public synchronized void connected(BluetoothSocket socket, BluetoothDevice
            device, final String socketType) {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "connected, Socket Type:" + socketType);

        // Cancel the thread that completed the connection
        if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}

        // Cancel any thread currently running a connection
        if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

        // Cancel the accept thread because we only want to connect to one device
        if (mSecureAcceptThread != null) {
        	mSecureAcceptThread.cancel();
            mSecureAcceptThread.interrupt();
            mSecureAcceptThread = null;
        }

        // Start the thread to manage the connection and perform transmissions
        mConnectedThread = new ConnectedThread(socket, socketType);
        mConnectedThread.start();

        // Send the name of the connected device back to the UI Activity
        Message msg = mHandler.obtainMessage(AppUtil.MESSAGE_DEVICE_NAME);
        Bundle bundle = new Bundle();
        bundle.putString(AppUtil.DEVICE_NAME, device.getName());
        msg.setData(bundle);
        mHandler.sendMessage(msg);

        setState(STATE_CONNECTED);
    }

    /**
     * Stop all threads
     */
    public synchronized void stop() {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "stop");
        
        if (mConnectThread != null) {
        	mConnectThread.interrupt();
        	mConnectThread.cancel();
        	mConnectThread = null;
        }
        
        if (mConnectedThread != null) {
        	mConnectedThread.cancel();
            mConnectedThread = null;
        }

        if (mSecureAcceptThread != null) {
            mSecureAcceptThread.cancel();
            mSecureAcceptThread = null;
        }

        setState(STATE_NONE);
    }

    /**
     * Write to the ConnectedThread in an unsynchronized manner
     * @param out The bytes to write
     * @see ConnectedThread#write(byte[])
     */
    public void write(byte[] out) {
        // Create temporary object
        ConnectedThread r;
        // Synchronize a copy of the ConnectedThread
        synchronized (this) {
            if (mState != STATE_CONNECTED) return;
            r = mConnectedThread;
        }
        // Perform the write unsynchronized
        r.write(out);
    }

    /**
     * Indicate that the connection attempt failed and notify the UI Activity.
     */
    private void connectionFailed() {
        // Send a failure message back to the Activity
        Message msg = mHandler.obtainMessage(AppUtil.MESSAGE_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(AppUtil.TOAST, "Unable to connect device");
        msg.setData(bundle);
        mHandler.sendMessage(msg);

        // Start the service over to restart listening mode
        BluetoothChatService.this.start();
    }

    /**
     * Indicate that the connection was lost and notify the UI Activity.
     */
    public void connectionLost() {
        // Send a failure message back to the Activity
        Message msg = mHandler.obtainMessage(AppUtil.MESSAGE_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(AppUtil.TOAST, "Device connection was lost");
        msg.setData(bundle);
        mHandler.sendMessage(msg);

        // Start the service over to restart listening mode
        BluetoothChatService.this.start();
    }

    /**
     * This thread runs while listening for incoming connections. It behaves
     * like a server-side client. It runs until a connection is accepted
     * (or until cancelled).
     */
    private class AcceptThread extends Thread {
        // The local server socket
        private final BluetoothServerSocket mmServerSocket;
        private String mSocketType;

        public AcceptThread(boolean secure) {
            BluetoothServerSocket tmp = null;

            // Create a new listening server socket
            try {
               tmp = mAdapter.listenUsingRfcommWithServiceRecord(NAME_SECURE, MY_UUID_SECURE);
            } catch (IOException e) {
                Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "listen() failed", e);
            }
            mmServerSocket = tmp;
        }

        public void run() {
            if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "Socket Type: " + mSocketType +
                    "BEGIN mAcceptThread" + this);
            setName("AcceptThread" + mSocketType);

            BluetoothSocket socket = null;

            // Listen to the server socket if we're not connected
            while (mState != STATE_CONNECTED) {
                try {
                    // This is a blocking call and will only return on a
                    // successful connection or an exception
                	if (mmServerSocket != null)
                		socket = mmServerSocket.accept();
                } catch (IOException e) {
                    //Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + " accept() failed", e);
                    break;
                }

                // If a connection was accepted
                if (socket != null) {
                    synchronized (BluetoothChatService.this) {
                        switch (mState) {
                        case STATE_LISTEN:
                        case STATE_CONNECTING:
                            // Situation normal. Start the connected thread.
                            connected(socket, socket.getRemoteDevice(),
                                    mSocketType);
                            break;
                        case STATE_NONE:
                        case STATE_CONNECTED:
                            // Either not ready or already connected. Terminate new socket.
                            try {
                                socket.close();
                            } catch (IOException e) {
                                Log.e(AppUtil.TAG, "Could not close unwanted socket", e);
                            }
                            break;
                        }
                    }
                }
            }
            if (AppUtil.DEBUG) Log.i(AppUtil.TAG, "END mAcceptThread, socket Type: " + mSocketType);

        }

        public void cancel() {
        	if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "Socket Type" + mSocketType + "cancel " + this);
            try {
            	if (mmServerSocket != null)
            		mmServerSocket.close();
            } catch (IOException e) {
                Log.e(AppUtil.TAG, "Socket Type" + mSocketType + "close() of server failed", e);
            }
            
        }
    }

    /**
     * This thread runs while attempting to make an outgoing connection
     * with a device. It runs straight through; the connection either
     * succeeds or fails.
     */
    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;
        private String mSocketType;

        public ConnectThread(BluetoothDevice device) {
            mmDevice = device;
            BluetoothSocket tmp = null;

            // Get a BluetoothSocket for a connection with the
            // given BluetoothDevice
            try {
               //tmp = device.createRfcommSocketToServiceRecord(MY_UUID_SECURE);
            	Method m = device.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
                tmp = (BluetoothSocket) m.invoke(device, Integer.valueOf(1));
            //} catch (IOException e) {
            //    Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "create() failed - IOException", e);
            } catch (NoSuchMethodException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "create() failed - NoSuchMethodException", e);
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "create() failed - IllegalArgumentExecption", e);
			} catch (IllegalAccessException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "create() failed - IllegalAccessException", e);
			} catch (InvocationTargetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.e(AppUtil.TAG, "Socket Type: " + mSocketType + "create() failed - InvocationTargetException", e);
			}
            mmSocket = tmp;
        }

        public void run() {
            Log.i(AppUtil.TAG, "BEGIN mConnectThread SocketType:" + mSocketType);
            setName("ConnectThread" + mSocketType);

            // Always cancel discovery because it will slow down a connection
            mAdapter.cancelDiscovery();

            // Make a connection to the BluetoothSocket
            try {
                // This is a blocking call and will only return on a
                // successful connection or an exception
                mmSocket.connect();
            } catch (IOException e) {
                // Close the socket
                try {
                    mmSocket.close();
                } catch (IOException e2) {
                    Log.e(AppUtil.TAG, "unable to close() " + mSocketType +
                            " socket during connection failure", e2);
                }
                connectionFailed();
                return;
            }

            // Reset the ConnectThread because we're done
            synchronized (BluetoothChatService.this) {
                mConnectThread = null;
            }

            // Start the connected thread
            connected(mmSocket, mmDevice, mSocketType);
        }

        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(AppUtil.TAG, "close() of connect " + mSocketType + " socket failed", e);
            }
        }
    }

    /**
     * This thread runs during a connection with a remote device.
     * It handles all incoming and outgoing transmissions.
     */
    private class ConnectedThread extends Thread {
        private BluetoothSocket mmSocket;
        private InputStream mmInStream;
        private OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket, String socketType) {
            Log.d(AppUtil.TAG, "create ConnectedThread: " + socketType);
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the BluetoothSocket input and output streams
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.e(AppUtil.TAG, "temp sockets not created", e);
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            Log.i(AppUtil.TAG, "BEGIN mConnectedThread");
            byte[] buffer = new byte[1024];
            int bytes, byteIndex = 0;
            Message m = null;
            
            // Keep listening to the InputStream while connected
            while (true) {
            	
            		boolean eom = false;
	        		try {
	    	        	while (!eom && (mmInStream != null)) {
	    	                // Read from bluetooth
	    	        		if (mmInStream.available() > 0)
	    	        		{
	    	        			bytes = mmInStream.read(buffer, byteIndex, 1024 - byteIndex);
	    	        			byteIndex += bytes;
	    	        			if (buffer[byteIndex - 1] == '\r')
	    	        				eom = true;
	    	        		}
	    	        	}
	    	        } catch (IOException e) {
	    	            Log.e(AppUtil.TAG, "io exception disconnected", e);
	    	            connectionLost();
	    	        } catch (Exception e) {
	    	        	Log.e(AppUtil.TAG, "disconnected", e);
	    	            connectionLost();
	    	        }
    	        	
	        		if (byteIndex > 0)
	        		{
	        			Log.i(AppUtil.TAG, "Received: " + new String(buffer, 0, (byteIndex - 1)));
	        			m = mHandler.obtainMessage(AppUtil.MESSAGE_READ, byteIndex - 1, -1, buffer);
	        			m.sendToTarget();
	        		}
                	byteIndex = 0;
            }
        }

        /**
         * Write to the connected OutStream.
         * @param buffer  The bytes to write
         */
        public void write(byte[] buffer) {
            try {
            	if (AppUtil.DEBUG) Log.i(AppUtil.TAG, "Sending: " + new String(buffer));

            	// write to bluetooth
                mmOutStream.write(buffer);
                mmOutStream.write("\r".getBytes("US-ASCII"));
                mmOutStream.flush();

	            // also write to UI
	            mHandler.obtainMessage(AppUtil.MESSAGE_WRITE, -1, -1, buffer).sendToTarget();
            } catch (IOException e) {
                Log.e(AppUtil.TAG, "Exception during write", e);

            }
        }

        public void cancel() {
			if (mmInStream != null) {
	                try {mmInStream.close();} catch (Exception e) {Log.e(AppUtil.TAG, "close() of input stream failed", e);}
	                mmInStream = null;
	        }
	
	        if (mmOutStream != null) {
	                try {mmOutStream.close();} catch (Exception e) {Log.e(AppUtil.TAG, "close() of output stream failed", e);}
	                mmOutStream = null;
	        }
	
	        if (mmSocket != null) {
	                try {mmSocket.close();} catch (Exception e) {Log.e(AppUtil.TAG, "close() of connect socket failed", e);}
	                mmSocket = null;
	        }
        }
    }
}
