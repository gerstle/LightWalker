package com.inappropirates.remotecontrol;

import com.inappropirates.remotecontrol.R;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

public class LightWalkerRemoteback extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_light_walker_remote);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.light_walker_remote, menu);
		return true;
	}

}
