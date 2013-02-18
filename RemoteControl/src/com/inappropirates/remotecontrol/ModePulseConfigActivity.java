package com.inappropirates.remotecontrol;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.inappropirates.remotecontrol.ColorPickerDialog.OnColorChangedListener;

public class ModePulseConfigActivity extends Activity {
	
	private Button mButton;
	private int mColor = Color.BLUE;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_mode_pulse_config);
		
		mButton = (Button)findViewById(R.id.buttonColor);
		mButton.setBackgroundColor(mColor);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_mode_pulse_config, menu);
		return true;
	}
	
	@Override
	public void onDestroy()
	{
		super.onDestroy();
		AppUtil.mLightWalker.sendMessage(AppUtil.ConstructMessage("pulse", "Color", AppUtil.Color2String(mColor)));
		AppUtil.mLightWalker.sendMessage(AppUtil.ConstructMessage("pulse", "MinPulseRate", ((EditText)findViewById(R.id.editTextMinPulseTime)).getText().toString()));
		AppUtil.mLightWalker.sendMessage(AppUtil.ConstructMessage("pulse", "MaxPulseRate", ((EditText)findViewById(R.id.editTextMaxPulseTime)).getText().toString()));
	}

	public void onColorButtonClick(View v) {
		PickColor(mColor);
	}
	
	public void PickColor(int oldColor) {
		OnColorChangedListener l = new OnColorChangedListener() {
            public void colorChanged(int color) {
            	//String message = "pulse color: ";
        		mColor = color;
        		mButton.setBackgroundColor(mColor);
        		
        		/*
        		AppUtil.mLightWalker.sendMessage(AppUtil.ConstructMessage("pulse", "color", AppUtil.Color2String(mColor)));

            	message += "red: " + Color.red(color) + "\n" +
                           "green: " + Color.green(color) + "\n" +
   			               "blue: " + Color.blue(color) + "\n";
            	Toast.makeText(ModePulseConfigActivity.this, message, Toast.LENGTH_SHORT).show();
            	*/
            }
        };
        
		ColorPickerDialog c = new ColorPickerDialog(this, l, oldColor);
    	c.show();
	}
}
