package com.inappropirates.remotecontrol;

import java.io.UnsupportedEncodingException;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.inappropirates.util.bluetooth.BluetoothChatService;
import com.inappropirates.util.bluetooth.DeviceListActivity;

@SuppressLint("HandlerLeak")
public class LightWalkerRemote extends Activity {
	
	private RadioGroup mRadioModeGroup;
  	private int mSelectedModeId;
  	//private Button modeConfigButton;
  	
    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
    private static final int REQUEST_ENABLE_BT = 3;
            
    private EditText mCommandText;
    private Button mSendButton;
    private StringBuffer mOutStringBuffer;
	  
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if(AppUtil.DEBUG) Log.e(AppUtil.TAG, "--Create--");
		
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_light_walker_remote);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.custom_title);
		
        // Set up the custom title
		AppUtil.mTitle = (TextView) findViewById(R.id.title_left_text);
		AppUtil.mTitle.setText(R.string.app_name);
		AppUtil.mTitle = (TextView) findViewById(R.id.title_right_text);
        
        AppUtil.mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // If the adapter is null, then Bluetooth is not supported
        if (AppUtil.mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
		mRadioModeGroup = (RadioGroup) findViewById(R.id.modeRadioGroup);
	}
	
    @Override
    public void onStart() {
        super.onStart();
        if(AppUtil.DEBUG) Log.e(AppUtil.TAG, "--START--");

        if (!AppUtil.mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        // Otherwise, setup the chat session
        } else {
        	if ((AppUtil.mChatService != null) && (AppUtil.mChatService.getState() != BluetoothChatService.STATE_CONNECTED)) {
       			AppUtil.mChatService.stop();
       			AppUtil.mChatService = null;
	            setupBluetooth();
        	} else if (AppUtil.mChatService == null)
        		setupBluetooth();
        }
    }

	@Override
    public synchronized void onResume() {
        super.onResume();
        if (AppUtil.DEBUG) Log.e(AppUtil.TAG, "--RESUME--");

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if ((AppUtil.mChatService != null) && (AppUtil.mChatService.getState() == BluetoothChatService.STATE_NONE)) {
    		// Only if the state is STATE_NONE, do we know that we haven't started already
        	AppUtil.mChatService.start();
        }
        
        AppUtil.mApplicationContext = getApplicationContext();
    }
    
    @Override
    public void onStop() {
    	super.onStop();
    	
    	if(AppUtil.DEBUG) Log.e(AppUtil.TAG, "--Stop--");
    }
    
    @Override
    public void onDestroy() {
    	super.onDestroy();
    	
        if (AppUtil.mBluetoothAdapter.isEnabled() && (AppUtil.mChatService != null)) {
        	AppUtil.mChatService.stop();
        }
    }

    private void setupBluetooth() {
    	// Initialize the compose field with a listener for the return key
        mCommandText = (EditText) findViewById(R.id.commandText);
        mCommandText.setOnEditorActionListener(mCommandListener);

        // Initialize the send button with a listener that for click events
        mSendButton = (Button) findViewById(R.id.buttonSend);
        mSendButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                // Send a message using content of the edit text widget
                TextView view = (TextView) findViewById(R.id.commandText);
                String message = view.getText().toString();
                sendMessage(message);
            }
        });
        
        AppUtil.mBluetoothMessageTextView = (TextView) findViewById(R.id.bluetoothMessageView);
        AppUtil.mBluetoothMessageLabel = getString(R.string.bluetooth_response);

        // Initialize the BluetoothChatService to perform bluetooth connections
        AppUtil.mChatService = new BluetoothChatService(this, AppUtil.mBluetoothHandler);
    }


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_light_walker_remote, menu);
		return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		Intent intent = null;
	    switch (item.getItemId()) {
	        case R.id.menu_bluetooth:
	        	intent = new Intent(this, DeviceListActivity.class);
	            startActivityForResult(intent, REQUEST_CONNECT_DEVICE_SECURE);
	            return true;
	        default:
	            return super.onOptionsItemSelected(item);
	    }
	}

	public void onConfigButtonClick(View v) {
	     // get selected radio button from radioGroup
		mSelectedModeId = mRadioModeGroup.getCheckedRadioButtonId();
		
		Intent intent;
		switch (mSelectedModeId)
		{
			case R.id.radioEqualizer:
				//Toast.makeText(LightWalkerRemote.this, "eq", Toast.LENGTH_SHORT).show();
				intent = new Intent(this, ModeEqualizerConfigActivity.class);
				startActivity(intent);
				break;
			case R.id.radioGravity:
				//Toast.makeText(LightWalkerRemote.this, "gravity", Toast.LENGTH_SHORT).show();
				intent = new Intent(this, ModeGravityConfigActivity.class);
				startActivity(intent);
				break;
			case R.id.radioSparkle:
				//Toast.makeText(LightWalkerRemote.this, "sparkle", Toast.LENGTH_SHORT).show();
				intent = new Intent(this, ModeSparkleConfigActivity.class);
				startActivity(intent);
				break;
		}
	}
	
    private TextView.OnEditorActionListener mCommandListener =
            new TextView.OnEditorActionListener() {
            public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
                // If the action is a key-up event on the return key, send the message
                if (actionId == EditorInfo.IME_NULL && event.getAction() == KeyEvent.ACTION_UP) {
                    String message = view.getText().toString();
                    sendMessage(message);
                }
                if(AppUtil.DEBUG) Log.i(AppUtil.TAG, "END onEditorAction");
                return true;
            }
        };
        
	private void sendMessage(String message) {
		// Check that we're actually connected before trying anything
        if (AppUtil.mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.title_not_connected, Toast.LENGTH_SHORT).show();
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
            mCommandText.setText(mOutStringBuffer);
        }
	}


    
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (AppUtil.DEBUG) Log.d(AppUtil.TAG, "onActivityResult " + resultCode);
        switch (requestCode) {
        case REQUEST_CONNECT_DEVICE_SECURE:
            // When DeviceListActivity returns with a device to connect
            if (resultCode == Activity.RESULT_OK) {
                // Get the device MAC address
                String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                // Get the BLuetoothDevice object
                BluetoothDevice device = AppUtil.mBluetoothAdapter.getRemoteDevice(address);
                // Attempt to connect to the device
                AppUtil.mChatService.connect(device);
                
                // Initialize the send button with a listener that for click events
                mSendButton = (Button) findViewById(R.id.buttonSend);
                mSendButton.setOnClickListener(new OnClickListener() {
                    public void onClick(View v) {
                        // Send a message using content of the edit text widget
                        TextView view = (TextView) findViewById(R.id.commandText);
                        String message = view.getText().toString();
                        sendMessage(message);
                    }
                });
                
                // Initialize the buffer for outgoing messages
                mOutStringBuffer = new StringBuffer("");
            }
            break;
        case REQUEST_ENABLE_BT:
            // When the request to enable Bluetooth returns
            if (resultCode == Activity.RESULT_OK) {
                // Bluetooth is now enabled, so set up a chat session
                setupBluetooth();
            } else {
                // User did not enable Bluetooth or an error occured
                Log.d(AppUtil.TAG, "BT not enabled");
                Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }
}
