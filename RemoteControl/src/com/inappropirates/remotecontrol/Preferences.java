package com.inappropirates.remotecontrol;

public enum Preferences {
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
	prefMainLegCount,
	prefMainPixelsPerLeg,
	prefMainMaxBrightness,
	prefMainDefaultMode,

	//------------------------------------------------------------------------
	// Mode
	//------------------------------------------------------------------------
	prefMode,

	//------------------------------------------------------------------------
	//Pulse
	//------------------------------------------------------------------------
	prefPulseMinRate,
	prefPulseMaxRate,
	prefPulseRandom,
	prefPulseSyncLegs,
	prefPulseColor,

	//------------------------------------------------------------------------
	// Sparkle
	//------------------------------------------------------------------------
	prefSparkleFootUpFadeRate,
	prefSparkleFootDownFadeRate,
	prefSparkleFlashLength,
	prefSparkleSparkleLength,
	prefSparkleFootFlashColor,
	prefSparkleFootSparkleColor,
	prefSparkleLegSparkleColor,

	//------------------------------------------------------------------------
	// Equalizer
	//------------------------------------------------------------------------
	prefEqualizerColor,
	prefEqualizerLevel,
	prefEqualizerRMSThreshold,
	prefEqualizerAllLights,
	
	//------------------------------------------------------------------------
	// Gravity
	//------------------------------------------------------------------------
	prefGravityXColor,
	prefGravityYColor,
	prefGravityZColor,
	prefGravityRotate,
}
