/*
  ==============================================================================

	FastSine.h
	Created: 26 Oct 2018 9:56:54pm
	Author:  Denver

  ==============================================================================
*/

#pragma once
#include <math.h>
#include "newFloatArray.h"
#define M_PI 3.14159265358979323846

static class DMath
{
public:
	DMath()
	{
		fillSineLookup();
	}

	int twoSum(int num)
	{
		int sol = 1;
		for (int i = 1; i < num; i++)
		{
			sol = (int)(sol + pow(2, i));
		}
		return sol;
	}

	int getBand(float freq)
	{
		if (freq <= bandOneRange)// < 23.46 Hz
		{
			return 1;
		}
		if ((freq <= 3 * bandOneRange) && freq > bandOneRange)// < 70.38 Hz
		{
			return 2;
		}
		if ((freq <= 7 * bandOneRange) && freq > (3 * bandOneRange))// < 164.22 Hz
		{
			return 3;
		}
		if ((freq <= 15 * bandOneRange) && freq > (7 * bandOneRange))// < 351.91 Hz
		{
			return 4;
		}
		if ((freq <= 31 * bandOneRange) && freq > (15 * bandOneRange))// < 727.27 Hz
		{
			return 5;
		}
		if ((freq <= 63 * bandOneRange) && freq > (31 * bandOneRange))// < 1478.01 Hz
		{
			return 6;
		}
		if ((freq <= 127 * bandOneRange) && freq > (63 * bandOneRange))// < 2979.47 Hz
		{
			return 7;
		}
		if ((freq <= 255 * bandOneRange) && freq > (127 * bandOneRange))// < 5982.41 Hz
		{
			return 8;
		}
		if ((freq <= 511 * bandOneRange) && freq > (255 * bandOneRange)) // < 11988.2644 Hz
		{
			return 9;
		}
		if (freq > 511 * bandOneRange)
		{
			return 10;
		}
	}

	float getFloorFreq(int band, float sampleRate)
	{
		if (band == 1)
		{
			return 0.0f;
		}
		if (band == 2)
		{
			return 23.4604105572f;
		}
		if (band == 3)
		{
			return 70.38123167f;
		}
		if (band == 4)
		{
			return 164.2228739f;
		}
		if (band == 5)
		{
			return 351.9061584f;
		}
		if (band == 6)
		{
			return 727.2727273f;
		}
		if (band == 7)
		{
			return 1478.005865f;
		}
		if (band == 8)
		{
			return 2979.472141f;
		}
		if (band == 9)
		{
			return 5982.404692f;
		}
		if (band == 10)
		{
			return 11988.26979f;
		}
		if (band == 11)
		{
			return sampleRate * 0.5f;
		}
	}

	float sine(double phase)
	{
		phase = phase - ((2 * M_PI)) * floor(phase / ((2 * M_PI)));
		int intp1 = (int)floor(phase / (2 * M_PI) * 100);
		int intp2 = 1 + intp1;
		float intratio = (float)(phase / (2 * M_PI) * 100) - intp1;

		return sineLookup[intp1] + intratio * (sineLookup[intp2] - sineLookup[intp1]);
	}

	int getTopBin(int band, float sampleRate)
	{
		float niqFreq = sampleRate * 0.5f;
		return (int)(niqFreq / (getFloorFreq(band+1, sampleRate)));
	}

	int nextPowerTwo(int num)
	{
		for (int i = 0; i < 15; i++)
		{
			if (num < pow(2, i))
			{
				return pow(2, i);
			}
		}
		return -1;
	}

private:
	void fillSineLookup()
	{
		for (int i = 0; i < 100; i++)
		{
			sineLookup[i] = (float)sin(i * 0.01 * 2.0 * M_PI);
		}
		sineLookup[100] = sineLookup[0];
	}

	float sineLookup[101];
	const float bandOneRange = 23.4604105572f;
};
