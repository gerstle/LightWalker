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
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.inappropirates.util.BluetoothChatService;

public class AppUtil extends Application {
	
	public static final boolean DEBUG = true;
	public static final String TAG = "LightWalkerRemote";
	public static Context context;
	
	public static final char keyDelimiter = '=';
	public static final char colorDelimiter = ',';
		
    public static BluetoothAdapter bluetoothAdapter = null;
    public static BluetoothChatService chatService = null;
    public static Context applicationContext = null;

 	public static TextView title;
    public static String connectedDeviceName = null;
    public static LightWalkerModes selectedMode;
    public static EditText commandText;
    private static StringBuffer outStringBuffer;
    
    public static LightWalkerRemote mLightWalker = null;
    
    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    
    public static ListLogger logger;
    
    //public static RMSThread mRMSThread = null;
    
	public static final Handler mBluetoothHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_STATE_CHANGE:
                if(DEBUG) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                switch (msg.arg1) {
                case BluetoothChatService.STATE_CONNECTED:
                    title.setText(R.string.title_connected_to);
                    title.append(connectedDeviceName);
                    
                    // Initialize the buffer for outgoing messages
                    outStringBuffer = new StringBuffer("");
                    String modeName = selectedMode.toString().toLowerCase(Locale.ENGLISH);
                    SharedPreferences pref = context.getSharedPreferences(modeName  + "_preferences", MODE_PRIVATE);
                    
                    AppUtil.setMode(selectedMode, pref, true);
                    break;
                case BluetoothChatService.STATE_CONNECTING:
                    title.setText(R.string.title_connecting);
                    break;
                case BluetoothChatService.STATE_LISTEN:
                case BluetoothChatService.STATE_NONE:
                    title.setText(R.string.title_not_connected);
                    break;
                }
                break;
            case MESSAGE_WRITE:
            	

                break;
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                // construct a string from the valid bytes in the buffer
                String readMessage = new String(readBuf, 0, msg.arg1);
                
                //if (bluetoothLog.getLineCount() > 100)
                	//bluetoothLog.setText("");
                logger.Log(readMessage);
                //bluetoothLog.setSelection(bluetoothLog.getText().length());
                
                if (readMessage.equals("SettingsPlease"))
                {
                	String modeName = selectedMode.toString().toLowerCase(Locale.ENGLISH);
                    AppUtil.sendModeSettings(context.getSharedPreferences("main_preferences", MODE_PRIVATE));
                    AppUtil.setMode(selectedMode, context.getSharedPreferences(modeName  + "_preferences", MODE_PRIVATE), true);
                }
                break;
            case MESSAGE_DEVICE_NAME:
                // save the connected device's name
                connectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(applicationContext, "Connected to "
                               + connectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case MESSAGE_TOAST:
                Toast.makeText(applicationContext, msg.getData().getString(TOAST), 
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
    	if (((mode != LightWalkerModes.main) && (mode != AppUtil.selectedMode)) || override)
    	{
    		// <cgerstle> quit processing things while we send commands
    		// give it it a second to chill.
    		// <cgerstle> fuck that.
    		//AppUtil.sendMessage(AppUtil.ConstructMessage("mode", String.valueOf(LightWalkerModes.masterOff.ordinal())));

			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    		AppUtil.selectedMode = mode;
    		AppUtil.sendModeSettings(prefs);
    		//AppUtil.sendMessage(AppUtil.ConstructMessage("mode", mode.toString()));
    		AppUtil.sendMessage(AppUtil.ConstructMessage("mode", String.valueOf(mode.ordinal())));
    	}
    }
    
	public static void sendMessage(String message) {
		//bluetoothLog.append(message.concat("\r\n"));
		//bluetoothLog.setSelection(bluetoothLog.getText().length());
		logger.Log(message);
		
		// Check that we're actually connected before trying anything
        if (AppUtil.chatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(context, R.string.title_not_connected, Toast.LENGTH_SHORT).show();
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
            AppUtil.chatService.write(send);

            // Reset out string buffer to zero and clear the edit text field
            outStringBuffer.setLength(0);
        }
	}
	
	public static void sendModeSettings(SharedPreferences prefs) {
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
				// gotta give the other end some time to pull it off
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
    }
    
    public static String ConstructMessage(String key, String value) {
    	return String.valueOf(Preferences.valueOf(key).ordinal()) + keyDelimiter + value;
    	// return key + mKeyDelimiter + value;
    }
    
    public static boolean keyIsColor(String key) {
    	if (key.contains("Color"))
    		return true;
    	else
    		return false;
    }
    
    public static String Color2String(int color) {
    	String rv = "";
    	
    	int newColor = Color.argb(Color.alpha(color), RoundColor(Color.red(color)), RoundColor(Color.green(color)), RoundColor(Color.blue(color)));
    	float[] hsvColor = new float[3];
    	Color.colorToHSV(newColor, hsvColor);
    	
    	rv += "" + (int)AppUtil.Map(hsvColor[0], 0, 360, 0, 255) + colorDelimiter +
    			   (int)AppUtil.Map(hsvColor[1], 0, 1, 0, 255) + colorDelimiter +
    			   (int)AppUtil.Map(hsvColor[2], 0, 1, 0, 255);
    	
    	Log.i(AppUtil.TAG, "setting color to: " + rv);
    	return rv;
    }
    
    final static int roundValue = 17;
    private static int RoundColor(int color) {
    	int modValue = color % roundValue;
      	if (modValue == 0)
    		return color;
    	
      	int cutoff = (int) (roundValue * 0.8);
      	if (modValue <= cutoff)
      		return color - modValue;
      	else
      		return color + (roundValue - modValue);
    }
    
    public static float Map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    	return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
    }
}