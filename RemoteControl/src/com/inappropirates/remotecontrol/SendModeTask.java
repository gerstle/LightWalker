package com.inappropirates.remotecontrol;

import java.util.Map;

import com.inappropirates.util.BluetoothChatService;

import android.content.SharedPreferences;
import android.os.AsyncTask;

class ModeConfigs {
	private LightWalkerModes mMode;
	private SharedPreferences mPreferences;
	private boolean mOverride = false;
	
	public LightWalkerModes getMode() {
		return mMode;
	}
	public void setMode(LightWalkerModes Mode) {
		this.mMode = Mode;
	}
	public SharedPreferences getPreferences() {
		return mPreferences;
	}
	public void setPreferences(SharedPreferences Preferences) {
		this.mPreferences = Preferences;
	}
	
	public boolean isOverride() {
		return mOverride;
	}
	public void setOverride(boolean Override) {
		this.mOverride = Override;
	}
	
	public ModeConfigs(LightWalkerModes mode, SharedPreferences prefs)
	{
		mMode = mode;
		mPreferences = prefs;
	}
	
	public ModeConfigs(LightWalkerModes mode, SharedPreferences prefs, boolean override)
	{
		mMode = mode;
		mPreferences = prefs;
		mOverride = override;
	}
}

class SendModeTask extends AsyncTask<ModeConfigs, String, Void> {
	@Override
	protected Void doInBackground(ModeConfigs... modes) {
		
		if (AppUtil.chatService.getState() != BluetoothChatService.STATE_CONNECTED)
			return null;
		
		String msg;
		
		for (int i = 0; i < modes.length; i++) {
			if ((modes[i].getMode() != AppUtil.selectedMode) || modes[i].isOverride()) {
				msg = AppUtil.ConstructMessage("mode", String.valueOf(modes[i].getMode().ordinal()));
	    		sendModeSettings(modes[i].getPreferences());
	    		AppUtil.sendMessage(msg, false);
	    		publishProgress(msg);
	    		
	    		if (modes[i].getMode() != LightWalkerModes.main)
					AppUtil.selectedMode = modes[i].getMode();
			}
        }
		return null;
	}
	
	protected void onProgressUpdate(String... progress) {
        AppUtil.logger.Log(progress[0]);
    }
	
	private void sendModeSettings(SharedPreferences prefs) {
		String msg;
		
    	Map<String,?> keys = prefs.getAll();

		String value = null;
		for(Map.Entry<String,?> entry : keys.entrySet()) {
			// <cgerstle> the colors have gotta be handled special like
			if (AppUtil.keyIsColor(entry.getKey()))
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
				msg = AppUtil.ConstructMessage(entry.getKey(), value);
				AppUtil.sendMessage(msg, false);
				publishProgress(msg);
                
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
}
