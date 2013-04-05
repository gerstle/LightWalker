package com.inappropirates.util;

// Author xdebugx.net (Jeremiah McLeod) 8-8-2010
// <cgerstle> Original by Jeremiah, I've removed his frequency calculation
// and introduced an RMS calculation 

import java.util.ArrayList;
import java.util.List;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Handler;

public class RMSThread extends Thread {

	public static final int FREQUENCY_CHANGE = 1;

	public int sampleRate = 44100;
	public int readSize = 40;
	public boolean recording; // variable to start or stop recording
	public int frequency; // the public variable that contains the frequency
							// value "heard", it is updated continually while
							// the thread is running.

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
		int RMS = 0;

		int RMSListSize = 1800;
		int RMSSum = 0;
		int RMSIndex = 0;
		int RMSMovingAverage = 0;
		List<Integer> RMSValues = new ArrayList<Integer>();

		bufferSize = AudioRecord.getMinBufferSize(sampleRate,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT) * 3; // get
																					// the
																					// buffer
																					// size
																					// to
																					// use
																					// with
																					// this
																					// audio
																					// record

		recorder = new AudioRecord(AudioSource.MIC, sampleRate,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
				bufferSize); // instantiate the AudioRecorder

		recording = true; // variable to use start or stop recording
		audioData = new short[bufferSize]; // short array that pcm data is put
											// into.

		while (recording) { // loop while recording is needed
			// check to see if the recorder has initialized yet.
			if (recorder.getState() == android.media.AudioRecord.STATE_INITIALIZED)
				if (recorder.getRecordingState() == android.media.AudioRecord.RECORDSTATE_STOPPED)
					recorder.startRecording(); // check to see if the Recorder
												// has stopped or is not
												// recording, and make it
												// record.

				else {
					// read the PCM audio data into the audioData array
					shortsReceived = recorder.read(audioData, 0, readSize);

					sum = 0;
					for (int i = 0; i < shortsReceived; i++)
						sum += audioData[i] * audioData[i];

					if (shortsReceived > 0)
						RMS = (int) Math.sqrt(sum / shortsReceived);

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

					// tell the UI to update if it wants.
					if (mHandler != null)
						mHandler.obtainMessage(RMSThread.FREQUENCY_CHANGE,
								RMS, RMSMovingAverage).sendToTarget();
				}// else recorder started

		} // while recording

		if (recorder.getState() == android.media.AudioRecord.RECORDSTATE_RECORDING)
			recorder.stop(); // stop the recorder before ending the thread
		recorder.release(); // release the recorders resources
		recorder = null; // set the recorder to be garbage collected.

	}// run
}// RMSThread