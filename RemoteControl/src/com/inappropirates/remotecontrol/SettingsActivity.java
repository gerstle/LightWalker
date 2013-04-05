package com.inappropirates.remotecontrol;

import java.util.Locale;
import android.annotation.SuppressLint;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.preference.PreferenceActivity;

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
        mMode = LightWalkerModes.valueOf(mModeName);
       	mPreferencesName = mModeName.toLowerCase(Locale.ENGLISH) + "_preferences";

        getPreferenceManager().setSharedPreferencesName(mPreferencesName);
        addPreferencesFromResource(getResources().getIdentifier(mPreferencesName, "xml", getPackageName()));
        mPrefs = getSharedPreferences(mPreferencesName, MODE_PRIVATE);
        mPrefs.registerOnSharedPreferenceChangeListener(spChanged);
        
        if (mMode != LightWalkerModes.Main)
        	setMode(mMode);
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
							value = Boolean.toString(sharedPrefs.getBoolean(key, false));
						} catch (ClassCastException e2) {
							
						}
					}
				}
			}
			
			if ((value != null) && (value.length() > 0))
				AppUtil.sendMessage(AppUtil.ConstructMessage(key, value));			
		}
    };
    
    private void setMode(LightWalkerModes mode) {
    	if ((mode != LightWalkerModes.Main) && (mode != AppUtil.mSelectedMode))
    	{
    		AppUtil.mSelectedMode = mode;
    		AppUtil.sendModeSettings(mMode, mPrefs);
    		AppUtil.sendMessage("mode=" + mode.toString().toLowerCase(Locale.ENGLISH));
    	}
    }
}
