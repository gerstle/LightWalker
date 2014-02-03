#include "WProgram.h"

#define ARM_MATH_CM4
#include <arm_math.h>
#include "audio.h"

float eqEMA = 0;
float eqEMAPeak = 0;
float eqNminus2 = 0;
float eqNminus1 = 0;
float frequencyWindow[MAX_BANDS + 1];
IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;

void equalizerBaseline(LWConfigs *config)
{
    float value;
    float eqValueTotal = 0;
    float eqValueIndex = 0;
    float peak = 0;
    float intensity, otherMean;

    spectrumSetup(config);
    samplingBegin();

    // <gerstle> get EQ_EMA_N # of samples...
    for (eqValueIndex = 0; eqValueIndex < EQ_EMA_N; eqValueIndex++)
    {
        // Calculate FFT if a full sample is available.
        // <gerstle> wait for a sample
        while (!samplingIsDone()) {}

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

            Serial.println(intensity);

            // <gerstle> if looking at all bands, then
            // we just want the max value out of all bands
            if (config->equalizer.allBands)
                value = max(value, intensity);
            else if (i == 0)
                value = intensity;

            eqValueTotal += value;
            peak = max(peak, value);
        }

        // Restart audio sampling.
        samplingBegin();

        eqValueTotal += value;
        peak = max(peak, value);
    }
    
    eqEMA = eqValueTotal / EQ_EMA_N;
    eqEMAPeak = peak;
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
//             Serial.print(intensity); Serial.print("\t"); 

            // <gerstle> if looking at all bands, then
            // we just want the max value out of all bands
            if (config->equalizer.allBands)
                level = max(level, intensity);
            else if (i == 0)
                level = intensity;

            eqValueTotal += level;
            peak = max(peak, level);
        }
//         Serial.print(level); Serial.print("\t"); 

        if ((level < eqNminus1) && (eqNminus1 > eqNminus2) && (level > eqEMA))
        {
            // <cgerstle> artificially inflate the max...
            tmp = (1024 - level) / 4 + level; 
            eqEMAPeak = (float)(tmp - eqEMAPeak) * ((float)2 / (float)(EQ_EMA_PEAK_N + 1)) + eqEMAPeak;
        }

        eqEMA = (float)(level - eqEMA) * ((float)2 / (float)(EQ_EMA_N + 1)) + eqEMA;
        eqNminus2 = eqNminus1;
        eqNminus1 = level;
        
//         if (level < config->equalizer.rmsThreshold)
//             level = 0;

//         Serial.print(level); Serial.print("\t"); Serial.print(eqEMA); Serial.print("\t"); Serial.print(eqEMAPeak); Serial.print("\t"); Serial.println(eqLevel);

        // Restart audio sampling.
        samplingBegin();

        return level;
    }
    else
        return eqNminus1;
}
