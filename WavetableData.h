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

	void fft(int band)
	{
		for (int w = 0; w < 256; w++)
		{
			fftObject.fft(&data[band][w][0], 2048);
		}
	}

	void ifft(int band)
	{
		for (int w = 0; w < 256; w++)
		{
			fftObject.ifft(&data[band][w][0], 2048);
		}
	}

private:
	float data[BANDS][256][2049];

	FFT fftObject;
};