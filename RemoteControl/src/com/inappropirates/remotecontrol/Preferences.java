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
	sparkleFootUpFadeRate,
	sparkleFootDownFadeRate,
	sparkleFlashLength,
	sparkleSparkleLength,
	sparkleFootFlashColor,
	sparkleFootSparkleColor,
	sparkleLegSparkleColor,

	//------------------------------------------------------------------------
	// Equalizer
	//------------------------------------------------------------------------
	eqRMSThreshold,
	eqColor,
	eqAllLights,
	eqAllBands,

	//------------------------------------------------------------------------
	// Gravity
	//------------------------------------------------------------------------
	gravityColorOne,
	gravityColorTwo,
	gravityColorThree,
	gravityRotate,
}