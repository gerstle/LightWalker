package com.inappropirates.remotecontrol;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.inappropirates.remotecontrol.ColorPickerDialog.OnColorChangedListener;

public class ModeGravityConfigActivity extends Activity {
	
	public enum Direction { NONE, X, Y, Z };

	private Integer mXColor = Color.BLUE;
	private Integer mYColor = Color.GREEN;
	private Integer mZColor = Color.RED;
	private Direction mDirection = Direction.NONE;
	//private boolean mRotate = false;
	
	private Button mXButton;
	private Button mYButton;
	private Button mZButton;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_mode_gravity_config);
		
		mXButton = (Button)findViewById(R.id.buttonX);
		mYButton = (Button)findViewById(R.id.buttonY);
		mZButton = (Button)findViewById(R.id.buttonZ);
		
		mXButton.setBackgroundColor(mXColor);
		mYButton.setBackgroundColor(mYColor);
		mZButton.setBackgroundColor(mZColor);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_mode_gravity_config, menu);
		return true;
	}

	public void onXColorButtonClick(View v) {
		mDirection = Direction.X;
		PickColor(mXColor);
	}
	
	public void onYColorButtonClick(View v) {
		mDirection = Direction.Y;
		PickColor(mYColor);
	}
	
	public void onZColorButtonClick(View v) {
		mDirection = Direction.Z;
		PickColor(mZColor);
	}
	
	public void PickColor(int oldColor) {
		OnColorChangedListener l = new OnColorChangedListener() {
            public void colorChanged(int color) {
            	String message = "";
            	switch (mDirection){
	            	case X:
	            		message = "X: ";
	            		mXColor = color;
	            		mXButton.setBackgroundColor(mXColor);
	            		mDirection = Direction.NONE;
	            		
	            		//AppUtil.sendMessage(AppUtil.ConstructMessage(LightWalkerModes.Gravity, "x", AppUtil.Color2String(mXColor)));
	            		break;
	            	case Y:
	            		message = "Y: ";
	            		mYColor = color;
	            		mYButton.setBackgroundColor(mYColor);
	            		mDirection = Direction.NONE;
	            		
	            		//AppUtil.sendMessage(AppUtil.ConstructMessage(LightWalkerModes.Gravity, "y", AppUtil.Color2String(mYColor)));
	            		break;
	            	case Z:
	            		message = "Z: ";
	            		mZColor = color;
	            		mZButton.setBackgroundColor(mZColor);
	            		mDirection = Direction.NONE;
	            		
	            		//AppUtil.sendMessage(AppUtil.ConstructMessage(LightWalkerModes.Gravity, "z", AppUtil.Color2String(mZColor)));
	            		break;
            	}
            	
            	message += "red: " + Color.red(color) + "\n" +
                           "green: " + Color.green(color) + "\n" +
   			               "blue: " + Color.blue(color) + "\n";
            	Toast.makeText(ModeGravityConfigActivity.this, message, Toast.LENGTH_SHORT).show();
            }
        };
        
		ColorPickerDialog c = new ColorPickerDialog(this, l, oldColor);
    	c.show();
	}
}
