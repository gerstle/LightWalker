/*
 * Bluetooth.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: cgerstle
 */

#include <Arduino.h>
#include "../comms/Bluetooth.h"
#include "../config/Enums.h"

namespace comms {

static const char one_str[] = "1";

Bluetooth::Bluetooth() {
}

void Bluetooth::init(LW *lw) {
	Serial.println("setting up bluetooth...");

	this->lw = lw;
	Serial1.begin(9600);

	Serial.println("done");
}

void Bluetooth::listen() {
    if (Serial1.available())
    {
        msg[msgIndex++] = (char)Serial1.read();

        if (msg[msgIndex - 1] == '\r')
        {
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                if (executeCommand(atoi(pKey), pValue, msgIndex - (pValue - pKey) - 1))
                    Serial1.print("OK\r");
                else
                    Serial1.print("?\r");
            }

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

bool Bluetooth::executeCommand(int key, char *value, int valueLen)
{
	Serial.printf("key: %d value: %s\r\n", key, value);
	int valueInt;

	switch (key)
	{
		// ------------------------------------------------------------------------
		// Main
		// ------------------------------------------------------------------------
		case mainMaxBrightness:
			lw->config.main.maxBrightness = atoi(value);
			LEDS.setBrightness(lw->config.main.maxBrightness);
			break;
		case mainDefaultMode:
			break;

		// ------------------------------------------------------------------------
		// Mode
		// ------------------------------------------------------------------------
		case mode:
			lw->setMode(static_cast<WalkingModeEnum>(atoi(value)));
			/*
			Serial.print("\nmode set to ");
			switch (lw->_mode)
			{
				case masterOff:
					Serial.println("off");
					break;
				case pulse:
					Serial.println("pulse");
					break;
				case sparkle:
					Serial.println("sparkle");
					break;
				case equalizer:
					Serial.println("equalizer");
					break;
				case gravity:
					Serial.println("gravity");
					break;
				case bubble:
					Serial.println("bubble");
					break;
				case rainbow:
					Serial.println("rainbow");
					break;
				case zebra:
					Serial.println("zebra");
					break;
				case chaos:
					Serial.println("chaos");
					break;
				case flames:
					Serial.println("flames");
					break;
				default:
					Serial.println("nada");
			}
			*/
			break;

		// ------------------------------------------------------------------------
		// Sparkle
		// ------------------------------------------------------------------------
		case sparkleFadeRate:
			lw->config.sparkle.fadeRate = atoi(value);
			break;
		case sparkleFlashLength:
			lw->config.sparkle.flashLength = atoi(value);
			break;
		case sparkleSparkleLength:
			lw->config.sparkle.sparkleLength = atoi(value);
			break;
		case sparkleFootFlashColor:
			this->parseColor(value, &(lw->config.sparkle.footFlashColor));
			break;
		case sparkleSparkleColor:
			this->parseColor(value, &(lw->config.sparkle.sparkleColor));
			break;
		case sparkleMinValue:
			lw->config.sparkle.minValue = atoi(value);
			break;

		// ------------------------------------------------------------------------
		// Pulse
		// ------------------------------------------------------------------------
		case pulseMinRate:
			lw->config.pulse.minPulseTime = atoi(value);
			break;
		case pulseMaxRate:
			lw->config.pulse.maxPulseTime = atoi(value);
			break;
		case pulseSyncLegs:
			if (strncmp(value, one_str, valueLen) == 0)
				lw->config.pulse.syncLegs = true;
			else
				lw->config.pulse.syncLegs  = false;
			break;
		case pulseColor:
			this->parseColor(value, &(lw->config.pulse.color));
			LWUtils.printRGB(lw->config.pulse.color, true);
			break;
		case pulseMode:
			lw->config.pulse.mode = static_cast<PulseMode>(atoi(value));
			//lw->resetPulseMode();
			break;

		// ------------------------------------------------------------------------
		// Equalizer
		// ------------------------------------------------------------------------
		case eqMode:
			lw->config.equalizer.mode = static_cast<EqualizerMode>(atoi(value));
			break;
		case eqColor:
			this->parseColor(value, &(lw->config.equalizer.color));
			break;
		case eqAllBands:
			if (strncmp(value, one_str, 1) == 0)
				lw->config.equalizer.allBands = true;
			else
				lw->config.equalizer.allBands = false;
			break;
		case eqMinValue:
			lw->config.equalizer.minValue = atoi(value);
			break;

		// ------------------------------------------------------------------------
		// Gravity
		// ------------------------------------------------------------------------
		case gravityRotate:
			if (strncmp(value, one_str, 1) == 0)
				lw->config.gravity.rotate = true;
			else
				lw->config.gravity.rotate = false;
			break;
		case gravityMinValue:
			lw->config.gravity.minValue= atoi(value);
			break;

		//------------------------------------------------------------------------
		// Bubble
		//------------------------------------------------------------------------
		case bubbleBackgroundColor:
			this->parseColor(value, &(lw->config.bubble.backgroundColor));
			break;
		case bubbleBubbleColor:
			this->parseColor(value, &(lw->config.bubble.bubbleColor));
			break;
		case bubbleSpeed:
			lw->config.bubble.speed = atoi(value);
			break;
		case bubbleWidth:
			lw->config.bubble.width = atoi(value);
			break;
		case bubbleTrail:
			if (strncmp(value, one_str, 1) == 0)
				lw->config.bubble.trail = true;
			else
				lw->config.bubble.trail = false;
			break;

		//------------------------------------------------------------------------
		// Rainbow
		//------------------------------------------------------------------------
		case rainbowMode:
			lw->config.rainbow.mode = static_cast<RainbowMode>(atoi(value));
			break;
		case rainbowMinValue:
			lw->config.rainbow.minValue= atoi(value);
			break;
		case rainbowDelay:
			lw->config.rainbow.delay = atoi(value);
			break;

		//------------------------------------------------------------------------
		// Zebra
		//------------------------------------------------------------------------
		case zebraColorOne:
			this->parseColor(value, &(lw->config.zebra.colorOne));
			break;
		case zebraColorTwo:
			this->parseColor(value, &(lw->config.zebra.colorTwo));
			break;

		//------------------------------------------------------------------------
		// Chaos
		//------------------------------------------------------------------------
		case chaosMinValue:
			lw->config.chaos.minValue = atoi(value);
			break;

		case chaosStepLength:
			lw->config.chaos.stepLength = atoi(value);
			break;

		case chaosSpeed:
			lw->config.chaos.speed = atoi(value);
			break;

		case chaosColor:
			this->parseColor(value, &(lw->config.chaos.color));
			break;

		case chaosSwing:
			lw->config.chaos.swing = atoi(value);
			break;

		case chaosSparse:
			lw->config.chaos.sparse = atoi(value);
			break;


		//------------------------------------------------------------------------
		// Flames
		//------------------------------------------------------------------------
		case flamesStepMillis:
			lw->config.flames.stepMillis = atoi(value);
			break;
		case flamesDelay:
			lw->config.flames.delay = atoi(value);
			break;

		default:
			Serial.println(" <------------- not recognized");
			return false;
			break;
	}

	return true;
}

void Bluetooth::parseColor(char *colorString, CHSV *color)
{
    char *value = colorString;
    char *pComma = strchr(colorString, ',');
    *pComma = '\0';

    (*color).hue = byte(atoi(value));

    value = pComma + 1;
    pComma = strchr(value, ',');
    *pComma = '\0';

    (*color).sat = byte(atoi(value));
    (*color).val = byte(atoi(pComma + 1));
}

Bluetooth::~Bluetooth() {
}

} /* namespace comms */
