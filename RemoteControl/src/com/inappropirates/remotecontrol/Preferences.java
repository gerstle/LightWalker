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
	//Mode
	//------------------------------------------------------------------------
	prefMode,

	//------------------------------------------------------------------------
	//Pulse
	//------------------------------------------------------------------------
	prefPulseMinRate,
	prefPulseMaxRate,
	prefPulseRandomColor,
	prefPulseSyncLegs,
	prefPulseColor,

	//------------------------------------------------------------------------
	//Sparkle
	//------------------------------------------------------------------------
	prefSparkleFootUpFadeRate,
	prefSparkleFootDownFadeRate,
	prefSparkleFlashLength,
	prefSparkleSparkleLength,
	prefSparkleFootFlashColor,
	prefSparkleFootSparkleColor,
	prefSparkleLegSparkleColor,

	//------------------------------------------------------------------------
	//Equalizer
	//------------------------------------------------------------------------
	prefEqualizerColor,
	prefEqualizerLevel,
	prefEqualizerRMSThreshold, // <cgerstle> NOT used by lightwalker, just for the remote
	prefEqualizerAllLights
}
