package com.inappropirates.remotecontrol;

import java.io.UnsupportedEncodingException;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.inappropirates.util.bluetooth.BluetoothChatService;

public class AppUtil extends Application {
	
	public static final boolean DEBUG = true;
	public static final String TAG = "LightWalkerRemote";
	public static Context mContext;
	
	public static final char mModeDelimiter = ':';
	public static final char mKeyDelimiter = '=';
	public static final char mColorDelimiter = ',';
		
    public static BluetoothAdapter mBluetoothAdapter = null;
    public static BluetoothChatService mChatService = null;
    public static Context mApplicationContext = null;

 	public static TextView mTitle;
    public static String mConnectedDeviceName = null;
    public static LightWalkerModes mSelectedMode;
    public static EditText mCommandText;
    private static StringBuffer mOutStringBuffer;
    
    public static LightWalkerRemote mLightWalker = null;
    
    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    
    public static TextView mBluetoothMessageTextView;
    public static String mBluetoothMessageLabel;
    
	public static final Handler mBluetoothHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_STATE_CHANGE:
                if(DEBUG) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                switch (msg.arg1) {
                case BluetoothChatService.STATE_CONNECTED:
                    mTitle.setText(R.string.title_connected_to);
                    mTitle.append(mConnectedDeviceName);
                    
                    // Initialize the buffer for outgoing messages
                    mOutStringBuffer = new StringBuffer("");
                	switch (mSelectedMode)
    	    		{
    	    			case Equalizer:
    	    				AppUtil.sendMessage("equalizer:GO");
    	    				break;
    	    			case Gravity:
    	    				AppUtil.sendMessage("gravity:GO");
    	    				break;
    	    			case Sparkle:
    	    				AppUtil.sendMessage("sparkle:GO");
    	    				break;
    	    			case Pulse:
    	    				AppUtil.sendMessage("pulse:GO");
    	    				break;
    	    		}       
                    break;
                case BluetoothChatService.STATE_CONNECTING:
                    mTitle.setText(R.string.title_connecting);
                    break;
                case BluetoothChatService.STATE_LISTEN:
                case BluetoothChatService.STATE_NONE:
                    mTitle.setText(R.string.title_not_connected);
                    break;
                }
                break;
            case MESSAGE_WRITE:
            	

                break;
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                // construct a string from the valid bytes in the buffer
                String readMessage = new String(readBuf, 0, msg.arg1);
                mBluetoothMessageTextView.setText(mBluetoothMessageLabel + readMessage);
                
                break;
            case MESSAGE_DEVICE_NAME:
                // save the connected device's name
                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(mApplicationContext, "Connected to "
                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case MESSAGE_TOAST:
                Toast.makeText(mApplicationContext, msg.getData().getString(TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };
    
	public static void sendMessage(String message) {
		// Check that we're actually connected before trying anything
        if (AppUtil.mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(mContext, R.string.title_not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = null;
			try {
				send = message.getBytes("US-ASCII");
			} catch (UnsupportedEncodingException e) {
				Log.d(AppUtil.TAG, "code not encode to ASCII");
				e.printStackTrace();
			}
            AppUtil.mChatService.write(send);
            AppUtil.mBluetoothMessageTextView.setText(AppUtil.mBluetoothMessageLabel);

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer.setLength(0);
            mCommandText.setText(mOutStringBuffer);
            try {
				Thread.sleep(50);
			} catch (InterruptedException e) {
				Log.d(AppUtil.TAG, "sleep interrupted");
				e.printStackTrace();
			}
        }
	}
    
    public static String ConstructMessage(LightWalkerModes mode, String key, String value) {
    	return mode.toString() + mModeDelimiter + key + mKeyDelimiter + value;
    }
    
    public static String Color2String(int color) {
    	String rv = "";
    	rv += "" + Color.red(color) + mColorDelimiter +
    			Color.green(color) + mColorDelimiter +
    			Color.blue(color);
    	
    	return rv;
    }
}