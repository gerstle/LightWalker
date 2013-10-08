#pragma once

enum WalkingModeEnum
{
    masterOff,
    main,
    gravity,
    equalizer,
    sparkle,
    pulse,
    bubble
};

enum Preferences
{
    // ------------------------------------------------------------------------
    // Main
    // ------------------------------------------------------------------------
    mainMinBrightness,
    mainMaxBrightness,
    mainLegsOn,
    mainArmsOn,
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
    pulseRandomColor,
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
    gravityRotate,

    //------------------------------------------------------------------------
    // Bubble
    //------------------------------------------------------------------------
    bubbleBackgroundColor,
    bubbleBubbleColor,
    bubbleSpeed,
    bubbleWidth,
    bubbleTrail
};
