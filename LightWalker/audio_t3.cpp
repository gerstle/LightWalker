#ifdef __MK20DX256__
#include "WProgram.h"

#define ARM_MATH_CM4
#include <arm_math.h>
#include "audio.h"

float eqNminus1 = 0;
float frequencyWindow[MAX_BANDS + 1];
IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;

int minCounter = 0;
int maxCounter = 0;
elapsedMillis adjustmentTimer = 0;

void audioSetup()
{
    analogReadResolution(ANALOG_READ_RESOLUTION);
    analogReadAveraging(ANALOG_READ_AVERAGING);
}

void equalizerBaseline(LWConfigs *config)
{
    float value;
    float eqValueTotal = 0;
    float eqValueIndex = 0;
    float peak = 0;
    float intensity, otherMean;

    spectrumSetup(config);
    samplingBegin();
}

// <gerstle> returns a byte between 0 and 100
float equalizerListen(LWConfigs *config)
{
    float level;
    float tmp;
    float intensity, otherMean;
    float eqValueTotal = 0;
    float peak = 0;

    // Calculate FFT if a full sample is available.
    if (samplingIsDone())
    {
        // Run FFT on sample data.
        arm_cfft_radix4_instance_f32 fft_inst;
        arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
        arm_cfft_radix4_f32(&fft_inst, samples);

        // Calculate magnitude of complex numbers output by the FFT.
        arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);

        // <gerstle> foreach sample, we've got spin through the bands
        for (int i = 0; i < config->main.bands; ++i)
        {
            windowMean(magnitudes, frequencyToBin(frequencyWindow[i]), frequencyToBin(frequencyWindow[i+1]), &intensity, &otherMean);

            // Convert intensity to decibels.
            intensity = 20.0 * log10(intensity);

            // Scale the intensity and clamp between 0 and 1.0.
            intensity -= config->main.spectrumMinDB;
            intensity = intensity < 0.0 ? 0.0 : intensity;
            intensity /= (config->main.spectrumMaxDB - config->main.spectrumMinDB);
            intensity = intensity > 1.0 ? 1.0 : intensity;

            // <gerstle> if looking at all bands, then
            // we just want the max value out of all bands
            if (config->equalizer.allBands)
                level = max(level, intensity);
            else if (i == 0)
                level = intensity;
        }
        //Serial.print(level); Serial.println("\t"); 
        eqNminus1 = level;
        
        if (level > 0.95)
            maxCounter++;
        if (level < 0.10)
            minCounter++;

        // <cgerstle> auto adjust the min and max DB levels
        if (adjustmentTimer >= 2000)
        {
            adjustmentTimer = 0;

//             Serial.print("                      "); Serial.println(minCounter);
//             Serial.print("                      "); Serial.println(maxCounter);

            if (minCounter > 5)
                config->main.spectrumMinDB -= 1.0;
            else if (minCounter <= 1)
                config->main.spectrumMinDB += 1.0;

            if (maxCounter > 4)
                config->main.spectrumMaxDB += 3.0;
            else if (maxCounter <= 1)
                config->main.spectrumMaxDB -= 0.5;

//             Serial.print("                               "); Serial.println(config->main.spectrumMinDB);
//             Serial.print("                               "); Serial.println(config->main.spectrumMaxDB);
            minCounter = 0;
            maxCounter = 0;
        }

        // Restart audio sampling.
        samplingBegin();

        return level;
    }
    else
        return eqNminus1;
}

// <gerstle> borrowed from adafruit's spectrum.ino example
void spectrumSetup(LWConfigs *config)
{
    // Set the frequency window values by evenly dividing the possible frequency
    // spectrum across the number of neo pixels.
    float windowSize = (SAMPLE_RATE_HZ / 2.0) / float(config->main.bands);
    for (int i = 0; i < (config->main.bands + 1); ++i)
        frequencyWindow[i] = i * windowSize;
}

// <gerstle> borrowed from adafruit's spectrum.ino example
void samplingCallback()
{
    // Read from the ADC and store the sample data
    samples[sampleCounter] = (float32_t)analogRead(AUDIO_PIN);

    // Complex FFT functions require a coefficient for the imaginary part of the input.
    // Since we only have real data, set this coefficient to zero.
    samples[sampleCounter + 1] = 0.0;

    // Update sample buffer position and stop after the buffer is filled
    sampleCounter += 2;
    if (sampleCounter >= FFT_SIZE * 2)
        samplingTimer.end();
}

// <gerstle> borrowed from adafruit's spectrum.ino example
void samplingBegin()
{
    // Reset sample buffer position and start callback at necessary rate.
    sampleCounter = 0;
    samplingTimer.begin(samplingCallback, 1000000/SAMPLE_RATE_HZ);
}

// <gerstle> borrowed from adafruit's spectrum.ino example
bool samplingIsDone()
{
    return sampleCounter >= (FFT_SIZE * 2);
}

// Compute the average magnitude of a target frequency window vs. all other frequencies.
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean)
{
    *windowMean = 0;
    *otherMean = 0;

    // Notice the first magnitude bin is skipped because it represents the
    // average power of the signal.
    for (int i = 1; i < (FFT_SIZE / 2); ++i)
    {
        if ((i >= lowBin) && (i <= highBin))
            *windowMean += magnitudes[i];
        else
            *otherMean += magnitudes[i];
    }

    *windowMean /= (highBin - lowBin) + 1;
    *otherMean /= ((FFT_SIZE / 2) - (highBin - lowBin));
}

// Convert a frequency to the appropriate FFT bin it will fall within.
int frequencyToBin(float frequency)
{
    float binFrequency = float(SAMPLE_RATE_HZ) / float(FFT_SIZE);
    return int(frequency / binFrequency);
}
#endif
