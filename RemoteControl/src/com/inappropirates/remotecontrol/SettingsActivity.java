package com.inappropirates.remotecontrol;

import java.util.Locale;

import android.annotation.SuppressLint;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.preference.PreferenceActivity;

import com.inappropirates.util.RMSThread;

@SuppressLint("DefaultLocale")
public class SettingsActivity extends PreferenceActivity {
	private LightWalkerModes mMode;
	private String mModeName = "";
	private String mPreferencesName = "";
	private SharedPreferences mPrefs;
	
	@SuppressWarnings("deprecation")
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
 
        // get the custom preferences name from the extra data in the intent
        mModeName = getIntent().getExtras().getString(LightWalkerRemote.EXTRA_MODE_NAME);
        mMode = LightWalkerModes.valueOf(mModeName.toLowerCase(Locale.ENGLISH));
       	mPreferencesName = mModeName.toLowerCase(Locale.ENGLISH) + "_preferences";

        getPreferenceManager().setSharedPreferencesName(mPreferencesName);
        addPreferencesFromResource(getResources().getIdentifier(mPreferencesName, "xml", getPackageName()));
        mPrefs = getSharedPreferences(mPreferencesName, MODE_PRIVATE);
        mPrefs.registerOnSharedPreferenceChangeListener(spChanged);
        
        if (mMode != LightWalkerModes.main)
        	AppUtil.setMode(mMode, mPrefs);
    }
    
    @Override
    public void onDestroy() {
		super.onDestroy();
		mPrefs.unregisterOnSharedPreferenceChangeListener(spChanged);
    }
    
    SharedPreferences.OnSharedPreferenceChangeListener spChanged = new SharedPreferences.OnSharedPreferenceChangeListener() {
		@Override
		public void onSharedPreferenceChanged(SharedPreferences sharedPrefs, String key) {
			String value = null;
			
			if (AppUtil.keyIsColor(key))
				value = AppUtil.Color2String(sharedPrefs.getInt(key, Color.BLACK));
			else
			{
				try {
					value = sharedPrefs.getString(key, "");
				} catch (ClassCastException e) {
					try {
						value = Integer.toString(sharedPrefs.getInt(key, 0));
					} catch (ClassCastException e1) {
						try {
							boolean boolValue = sharedPrefs.getBoolean(key, false);
							value = boolValue ? "1" : "0";
						} catch (ClassCastException e2) {
							
						}
					}
				}
			}
			
			if ((value != null) && (value.length() > 0))
				AppUtil.sendMessage(AppUtil.ConstructMessage(key, value));
			
			if (key.compareTo("prefEqualizerRMSThreshold") == 0)
				AppUtil.mRMSThread.mRMSThreshold = sharedPrefs.getInt(key,  200);
			else if (key.compareTo("prefEqualizerFrequencyThreshold") == 0)
				AppUtil.mRMSThread.mFrequencyThreshold = sharedPrefs.getInt(key,  20);
		}
    };
}
