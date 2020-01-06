/*
  ==============================================================================

	WavetableData.h
	Created: 25 Oct 2018 3:38:22pm
	Author:  Denver

  ==============================================================================
*/

#pragma once
#include "fft.h"
#define BANDS 11

class WavetableData
{
public:
	void inputData(int band, int wave, int samp, float input)
	{
		data[band][wave][samp] = input;
	}

	float readData(int band, int wave, int samp)
	{
		return data[band][wave][samp];
	}

	void clearData()
	{
		for (int band = 0; band < BANDS; band++)
		{
			for (int w = 0; w < 256; w++)
			{
				for (int s = 0; s < 2049; s++)
				{
					inputData(band, w, s, 0.0f);
				}
			}
		}
	}

	void copyFromZero()
	{
		for (int b = 1; b < BANDS; b++)
		{
			for (int w = 0; w < 256; w++)
			{
				for (int s = 0; s < 2049; s++)
				{
					data[b][w][s] = data[0][w][s];
				}
			}
		}
	}

	void fft(int band)
	{
		for (int w = 0; w < 256; w++)
		{
			wavetableFFT[band][w].fft(&data[band][w][0], 2048);
		}
	}

	void ifft(int band)
	{
		for (int w = 0; w < 256; w++)
		{
			wavetableFFT[band][w].ifft();
			wavetableFFT[band][w].returnWave(&data[band][w][0], 2048);
			data[band][w][2048] = data[band][w][0];
		}
	}

	void removeHarmonics(int band, int startHarmonic, int endHarmonic)
	{
		for (int w = 0; w < 256; w++)
		{
			wavetableFFT[band][w].removeHarmonics(startHarmonic, endHarmonic);
		}
	}

	float getReal(int band, int table, int harmonic)
	{
		return wavetableFFT[band][table].getReal(harmonic);
	}

	float getImag(int band, int table, int harmonic)
	{
		return wavetableFFT[band][table].getImag(harmonic);
	}

	void setReal(int band, int table, int harmonic, float value)
	{
		wavetableFFT[band][table].setReal(harmonic, value);
	}

	void setImag(int band, int table, int harmonic, float value)
	{
		wavetableFFT[band][table].setImag(harmonic, value);
	}

private:
	float data[BANDS][256][2049];

	FFT wavetableFFT[BANDS][256];
};