package com.inappropirates.util;

// Author xdebugx.net (Jeremiah McLeod) 8-8-2010
// <cgerstle> Original by Jeremiah, I've removed his frequency calculation
// and introduced an RMS calculation 

import java.util.ArrayList;
import java.util.List;

import root.gast.audio.processing.ZeroCrossing;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Handler;

import com.inappropirates.remotecontrol.AppUtil;

public class RMSThread extends Thread {

	public static final int FREQUENCY_CHANGE = 1;

	public int mSampleRate = 16000; //44100;
	public int mReadSize = 40;
	public boolean mRecording; // variable to start or stop recording
	public int mRMSThreshold = 200;
	public int mFrequencyThreshold = 20;


	private Handler mHandler = null;

	public RMSThread(Handler handler) {
		mHandler = handler;
	}

	@Override
	public void run() {
		AudioRecord recorder;
		short audioData[];
		int bufferSize;
		int shortsReceived;
		double sum = 0;
		int numCrossing = 0;
		int RMS = 0;
		int RMSListSize = 1800;
		int RMSSum = 0;
		int RMSIndex = 0;
		float RMSMovingAverage = 0;
		List<Integer> RMSValues = new ArrayList<Integer>();
		long lastSendTime = 0;
		long currentTime = 0;
		float numSecondsRecorded = 0;
        float numCycles = 0;
        int frequency = 0;

		// get the buffer size to use with this audio record
		bufferSize = AudioRecord.getMinBufferSize(mSampleRate,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT) * 3; 

		recorder = new AudioRecord(AudioSource.MIC, mSampleRate,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
				bufferSize); // instantiate the AudioRecorder

		mRecording = true; // variable to use start or stop recording
		audioData = new short[bufferSize]; // short array that pcm data is put
											// into.

		while (mRecording) { // loop while recording is needed
			// check to see if the recorder has initialized yet.
			if (recorder.getState() == android.media.AudioRecord.STATE_INITIALIZED)
				if (recorder.getRecordingState() == android.media.AudioRecord.RECORDSTATE_STOPPED)
					recorder.startRecording(); // check to see if the Recorder
												// has stopped or is not
												// recording, and make it
												// record.

				else {
					// read the PCM audio data into the audioData array
					shortsReceived = recorder.read(audioData, 0, mReadSize);
					//int frequency = ZeroCrossing.calculate(mSampleRate, audioData);

					sum = numCrossing = 0;
					for (int i = 0; i < shortsReceived; i++)
					{
						sum += audioData[i] * audioData[i];
						
						if ((audioData[i] > 0 && audioData[i + 1] <= 0) || (audioData[i] < 0 && audioData[i + 1] >= 0))
							numCrossing++;
					}

					if (shortsReceived > 0)
					{
						RMS = (int) Math.sqrt(sum / shortsReceived);
						
						numSecondsRecorded = (float) shortsReceived/(float) mSampleRate;
				        numCycles = numCrossing/2;
				        frequency = (int) (numCycles/numSecondsRecorded);
					}

					// <cgerstle> seed the moving average
					if (RMSValues.size() < RMSListSize) {
						RMSValues.add(RMS);
						RMSSum += RMS;
						RMSIndex++;
						RMSMovingAverage = RMSSum / RMSListSize;
					} else {
						if (RMSIndex >= RMSListSize)
							RMSIndex = 0;

						RMSSum = RMSSum - RMSValues.get(RMSIndex) + RMS;
						RMSValues.set(RMSIndex, RMS);
						RMSMovingAverage = RMSSum / RMSListSize;
						RMSIndex++;
					}

					currentTime = System.currentTimeMillis();
					//if ((RMS < mRMSThreshold) || (frequency > mFrequencyThreshold))
					if (RMS < mRMSThreshold)
						RMS = 0;
					
					if ((RMSMovingAverage > 0) && (currentTime > (lastSendTime + 15)))
					//if (RMSMovingAverage > 0)
					{
						AppUtil.sendMessage(AppUtil.ConstructMessage("prefEqualizerLevel", String.valueOf(Math.min(100, Math.round(((float) RMS / (RMSMovingAverage * 2)) * 100)))));
						lastSendTime = currentTime;
					}
				}// else recorder started

		} // while recording

		if (recorder.getState() == android.media.AudioRecord.RECORDSTATE_RECORDING)
			recorder.stop(); // stop the recorder before ending the thread
		recorder.release(); // release the recorders resources
		recorder = null; // set the recorder to be garbage collected.

	}// run
}// RMSThread