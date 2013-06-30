package com.inappropirates.remotecontrol;

import java.io.UnsupportedEncodingException;
import java.util.Locale;
import java.util.Map;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.inappropirates.util.BluetoothChatService;

public class AppUtil extends Application {
	
	public static final boolean DEBUG = true;
	public static final String TAG = "LightWalkerRemote";
	public static Context mContext;
	
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
    
    //public static RMSThread mRMSThread = null;
    
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
                    String modeName = mSelectedMode.toString().toLowerCase(Locale.ENGLISH);
                    SharedPreferences pref = mContext.getSharedPreferences(modeName  + "_preferences", MODE_PRIVATE);
                    
                    AppUtil.setMode(mSelectedMode, pref, true);
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
                
                if (readMessage.equals("SettingsPlease"))
                {
                	String modeName = mSelectedMode.toString().toLowerCase(Locale.ENGLISH);
                    SharedPreferences pref = mContext.getSharedPreferences(modeName  + "_preferences", MODE_PRIVATE);
                    AppUtil.setMode(mSelectedMode, pref, true);
                }
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
    
    /*
    public static final Handler mRMSHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case RMSThread.FREQUENCY_CHANGE:
            	if (AppUtil.mChatService.getState() == BluetoothChatService.STATE_CONNECTED)
            		AppUtil.sendMessage(AppUtil.ConstructMessage("prefEqualizerLevel", String.valueOf((int) Math.round((msg.arg1 / (msg.arg2 * 3)) * 100))));
            	break;
            }
        }
    };
    */
    
    public static void setMode(LightWalkerModes mode, SharedPreferences prefs) {
    	AppUtil.setMode(mode, prefs, false);
    }
    
    public static void setMode(LightWalkerModes mode, SharedPreferences prefs, boolean override) {
    	if (((mode != LightWalkerModes.main) && (mode != AppUtil.mSelectedMode)) || override)
    	{
    		//<cgerstle> quit processing things while we send commands
    		// give it it a second to chill.
    		AppUtil.sendMessage(AppUtil.ConstructMessage("mode", "masterOff"));

			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    		AppUtil.mSelectedMode = mode;
    		AppUtil.sendModeSettings(mode, prefs);
    		AppUtil.sendMessage(AppUtil.ConstructMessage("mode", mode.toString()));
    	}
    }
    
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

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer.setLength(0);
        }
	}
	
	public static void sendModeSettings(LightWalkerModes mode, SharedPreferences prefs) {
    	Map<String,?> keys = prefs.getAll();

		String value = null;
		for(Map.Entry<String,?> entry : keys.entrySet()) {
			// <cgerstle> the colors have gotta be handled special like
			if (keyIsColor(entry.getKey()))
				value = AppUtil.Color2String((Integer)entry.getValue());
			else
			{
				Object o = entry.getValue();
				if (o instanceof Integer)
					value = o.toString();
				else if (o instanceof String)
					value = (String) o;
				else if (o instanceof Boolean)
					value = ((Boolean) o) ? "1" : "0";
			}
			
			if ((value != null) && (value.length() > 0))
			{
				sendMessage(AppUtil.ConstructMessage(entry.getKey(), value));
                
				// <cgerstle> when you're sending a bunch of data, you've
				// gotta give the other some time to pull it off
				try {
					Thread.sleep(150);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
    }
    
    public static String ConstructMessage(String key, String value) {
    	return key + mKeyDelimiter + value;
    }
    
    public static boolean keyIsColor(String key) {
    	if (key.contains("Color"))
    		return true;
    	else
    		return false;
    }
    
    public static String Color2String(int color) {
    	String rv = "";
    	rv += "" + Color.red(color) + mColorDelimiter +
    			Color.green(color) + mColorDelimiter +
    			Color.blue(color);
    	
    	return rv;
    }
}