#pragma once

enum WalkingModeEnum
{
    masterOff,
    main,
    pulse,
    sparkle,
    equalizer,
    gravity,
    bubble,
    rainbow,
    chaos,
    flames
};

enum Preferences
{
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
    mainMaxBrightness,
    mainDefaultMode,

    //------------------------------------------------------------------------
    //Mode
    //------------------------------------------------------------------------
    mode,

    //------------------------------------------------------------------------
    // Pulse
    //------------------------------------------------------------------------
	pulseMinRate,
	pulseMaxRate,
	pulseSyncLegs,
	pulseColor,
	pulseMode,

    //------------------------------------------------------------------------
    // Sparkle
    //------------------------------------------------------------------------
    sparkleFadeRate,
    sparkleFlashLength,
    sparkleSparkleLength,
    sparkleFootFlashColor,
    sparkleSparkleColor,
    sparkleMinValue,

    //------------------------------------------------------------------------
    // Equalizer
    //------------------------------------------------------------------------
    eqRMSThreshold,
    eqColor,
    eqAllLights,
    eqAllBands,
    eqRainbow,
    eqMinValue,

    //------------------------------------------------------------------------
    // Gravity
    //------------------------------------------------------------------------
    gravityRotate,
    gravityMinValue,

    //------------------------------------------------------------------------
    // Bubble
    //------------------------------------------------------------------------
    bubbleBackgroundColor,
    bubbleBubbleColor,
    bubbleSpeed,
    bubbleWidth,
    bubbleTrail,
    
    //------------------------------------------------------------------------
    // Rainbow
    //------------------------------------------------------------------------
    rainbowMode,
    rainbowMinValue,
    rainbowDelay
    
    //------------------------------------------------------------------------
    // Chaos
    //------------------------------------------------------------------------
    
    //------------------------------------------------------------------------
    // Flames
    //------------------------------------------------------------------------
};
