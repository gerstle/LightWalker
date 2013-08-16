package com.inappropirates.remotecontrol;

public enum Preferences {
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
    mainMinBrightness,
    mainMaxBrightness,
    mainLegsOn,
    mainArmsOn,
    mainDefaultMode,

	//------------------------------------------------------------------------
	// Mode
	//------------------------------------------------------------------------
	mode,

	//------------------------------------------------------------------------
	//Pulse
	//------------------------------------------------------------------------
	pulseMinRate,
	pulseMaxRate,
	pulseRandom,
	pulseSyncLegs,
	pulseColor,

	//------------------------------------------------------------------------
	// Sparkle
	//------------------------------------------------------------------------
	sparkleFadeRate,
	sparkleFlashLength,
	sparkleSparkleLength,
	sparkleFootFlashColor,
	sparkleSparkleColor,

	//------------------------------------------------------------------------
	// Equalizer
	//------------------------------------------------------------------------
	eqRMSThreshold,
	eqColor,
	eqAllLights,
	eqAllBands,
	eqRainbow,

	//------------------------------------------------------------------------
	// Gravity
	//------------------------------------------------------------------------
	gravityColorOne,
	gravityColorTwo,
	gravityColorThree,
	gravityRotate,
}