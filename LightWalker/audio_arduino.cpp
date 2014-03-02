#ifndef __MK20DX256__
#include "audio.h"

float eqEMA;
float eqEMAPeak;
int eqNminus2;
int eqNminus1;
unsigned long audioCurrentTime = millis();
unsigned long lastEQReading = millis();
int rmsThreshhold = 50;

void audioSetup()
{
    pinMode(AUDIO_STROBE_PIN, OUTPUT);
    pinMode(AUDIO_RESET_PIN, OUTPUT);
}

void equalizerBaseline(LWConfigs *config)
{
    int tmp, value;
    int eqValueTotal = 0;
    int eqValueIndex = 0;
    int peak = 0;

    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);
    for (eqValueIndex = 0; eqValueIndex < EQ_EMA_N; eqValueIndex++)
    {
        for (int i = 0; i < 7; i++)
        {
            digitalWrite(AUDIO_STROBE_PIN, LOW);
            delayMicroseconds(5); // to allow the output to settle
            tmp = analogRead(AUDIO_PIN);

            if (config->equalizer.allBands)
                value = max(value, tmp);
            else if (i == 0)
                value = tmp;

            digitalWrite(AUDIO_STROBE_PIN, HIGH);
        }
        eqValueTotal += value;
        peak = max(peak, value);
    }
    
    eqEMA = eqValueTotal / EQ_EMA_N;
    eqEMAPeak = peak;
    lastEQReading = millis();
}

// <gerstle> returns a byte between 0 and 100
float equalizerListen(LWConfigs *config)
{
    int level = 0;
    int tmp;
    float brightness;
    static bool emaCheck = true;

    audioCurrentTime = millis();
    if (audioCurrentTime <= (lastEQReading + 3))
        return eqNminus1;
    lastEQReading = audioCurrentTime;

    // <gerstle> get the value
    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(AUDIO_STROBE_PIN, LOW);
        delayMicroseconds(3); // to allow the output to settle

        tmp = analogRead(AUDIO_PIN);
        if (config->equalizer.allBands)
            level = max(level, tmp);
        else if (i == 0)
            level = tmp;
        digitalWrite(AUDIO_STROBE_PIN, HIGH);
    }

    if ((level < eqNminus1) && (eqNminus1 > eqNminus2) && (level > eqEMA))
    {
        // <cgerstle> artificially inflate the max...
        tmp = (1024 - level) / 4 + level; 
        eqEMAPeak = (float)(tmp - eqEMAPeak) * ((float)2 / (float)(EQ_EMA_PEAK_N + 1)) + eqEMAPeak;
    }

    if (emaCheck)
        eqEMA = (float)(level - eqEMA) * ((float)2 / (float)(EQ_EMA_N + 1)) + eqEMA;
    emaCheck = !emaCheck;

    eqNminus2 = eqNminus1;
    eqNminus1 = level;
    
    if (level < rmsThreshhold)
        level = 0;

    brightness = (float)map(level, eqEMA, eqEMAPeak, 0, 100) / (float)100;
    if (brightness < 0.0)
        brightness = 0;

    //Serial.print(level); Serial.print("\t"); Serial.print(eqEMA); Serial.print("\t"); Serial.print(eqEMAPeak); Serial.print("\t"); Serial.println(brightness);

    return brightness;
}
#endif
