/*****************************************************************************
 * LW.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LW.h"

// ----------------------------------------------------------------------------
// LW Class
// ----------------------------------------------------------------------------

void LW::initLegs(WalkingModeEnum m)
{
    _legs[0].Init(&config, "left leg", 2, mode);
    Serial.println(_legs[0].name);
    _legs[1].Init(&config, "right leg", 3, mode);
    Serial.println(_legs[1].name);

    mode = m;
    setMode(mode);
}

void LW::setMode(WalkingModeEnum m)
{
    mode = m;
    _lightModeChangeTime = millis();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(mode);

    switch (mode)
    {
        case equalizer:
            int tmp, value;

            valueTotal = 0;

            digitalWrite(AUDIO_RESET_PIN, HIGH);
            digitalWrite(AUDIO_RESET_PIN, LOW);
            for (valueIndex = 0; valueIndex < VALUE_COUNT; valueIndex++)
            {
                for (int i = 0; i < 7; i++)
                {
                    digitalWrite(AUDIO_STROBE_PIN, LOW);
                    delayMicroseconds(30); // to allow the output to settle
                    tmp = analogRead(AUDIO_LEFT_PIN);

                    if (i == 0)
                        value = tmp;

                    digitalWrite(AUDIO_STROBE_PIN, HIGH);
                }
                values[valueIndex] = value;
                valueTotal += values[valueIndex];
            }
            
            valueAvg = valueTotal / VALUE_COUNT;
            break;
    }
}

void LW::off()
{
    TCL.sendEmptyFrame();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

// <gerstle> check for inputs
void LW::walk()
{
    TCL.sendEmptyFrame();

    unsigned long currentTime = millis();
    bool displayStatus = false;
    bool pulseChangeColor = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = false;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        bool sensorState = digitalRead(_legs[i].trigger_pin);
        
        if (displayStatus && DEBUG)
        {
//             Serial.print("    leg "); Serial.println(_legs[i].name);
//             Serial.print("        pin: "); Serial.println(_legs[i].trigger_pin);
//             Serial.print("        pin state: "); Serial.println(sensorState);
//             Serial.print("        status: "); Serial.println(_legs[i].status);
            _laststatus = millis();
        }

        switch (mode)
        {
            case sparkle:

                //if ((sensorState == LOW) && (_legs[i].status != Down))
                if ((sensorState == HIGH) && (_legs[i].status != Down))
                    _legs[i].sparkle_footdown();
                //else if ((sensorState == HIGH) && (_legs[i].status != Up))
                else if ((sensorState == LOW) && (_legs[i].status != Up))
                    _legs[i].sparkle_footup();
                else
                    _legs[i].sparkle_sameStatus();

                _legs[i].sparkle_fade_on = true;
                break;

            case gravity:
                break;

            case equalizer:
                equalizer_listen();
                break;

            case pulse:
                if (config.pulse.syncLegs && (currentTime >= (_lightModeChangeTime + (_pulse_length * 2))))
                {
                    _pulse_length = random(config.pulse.minPulseTime, config.pulse.maxPulseTime);
                    _lightModeChangeTime = currentTime;
                    _pulse_isDimming = false;
                    pulseChangeColor = true;
                }
                else if (currentTime >= (_lightModeChangeTime + _pulse_length))
                    _pulse_isDimming = true;
                
                int value;
                if (config.pulse.syncLegs)
                    if (_pulse_isDimming)
                        value = (_pulse_length * 2) - (currentTime - _lightModeChangeTime);
                    else
                        value = currentTime - _lightModeChangeTime;

                _legs[i].pulse_pulse(currentTime, _lightModeChangeTime, _pulse_length, value, pulseChangeColor);
                break;
        }
    }
    TCL.sendEmptyFrame();
}

void LW::equalizer_listen()
{
    int level;
    int tmp;
    byte r, g, b = 0;
    float brightness;

    // <gerstle> get the value
    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(AUDIO_STROBE_PIN, LOW);
        delayMicroseconds(30); // to allow the output to settle

        tmp = analogRead(AUDIO_LEFT_PIN);
        if (i == 0)
            level = tmp;
        digitalWrite(AUDIO_STROBE_PIN, HIGH);
    }

    if (valueIndex >= VALUE_COUNT)
        valueIndex = 0;

    valueTotal = valueTotal - values[valueIndex] + level;
    values[valueIndex] = level;
    valueAvg = valueTotal / VALUE_COUNT;

//     Serial.print(config.equalizer.RMSThreshold); Serial.print("\t"); Serial.print(level); Serial.print("\t"); Serial.print(valueAvg);

    if (valueAvg <= 0)
        return;
    
    if (level < config.equalizer.RMSThreshold)
        level = 0;

    brightness = (float)level / ((float)valueAvg * 2);
    //Serial.print("\t"); Serial.print(level); Serial.print("\t"); Serial.println(brightness); 
    if (config.equalizer.allLights)
    {
        r = byte((float)config.equalizer.color.r * (brightness * brightness * brightness));
        g = byte((float)config.equalizer.color.g * (brightness * brightness * brightness));
        b = byte((float)config.equalizer.color.b * (brightness * brightness * brightness));
    }

    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].equalizer_listen(brightness, r, g, b);

    valueIndex++;
}
