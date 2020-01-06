/*
  ==============================================================================

	Wavetable.h
	Created: 8 Oct 2018 7:36:58pm
	Author:  Denver

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "WavetableData.h"
#include "DMath.h"
#include "WAVRead.h"
#include "newFloatArray.h"
#include "fft.h"
#define BANDS 11

class Wavetable
{
public:
	Wavetable()
	{
		importWavetable("C:\\Users\\denve\\OneDrive\\Desktop\\Wavetables\\ModernTalking.wav", 3, 3);
	}
	//============================================================

	float getSampValue(int band, int waveNumber, int samp)
	{
		return table.readData(band, waveNumber, samp);
	}
	//============================================================

	void importWavetable(char fileName[], int importType, int morphType)
	{
		int numSamples;
		int channels;

		WAVRead WAVFile(fileName);

		numSamples = WAVFile.getNumSamples();
		channels = WAVFile.getNumChannels();
		
		if (numSamples < 2)
		{
			throw std::runtime_error("Import Failed");
			return;
		}

		if (channels < 1)
		{
			throw std::runtime_error("Import Failed");
			return;
		}


		table.clearData();

		if (importType == 1) // Split WAV into 256 even segments.
		{
			int wtPeriod = (int)((numSamples * channels) / 256);
			constPeriodImport(wtPeriod, numSamples * channels, &WAVFile, morphType);
		}

		else if (importType == 2) // Single Cycle Import
		{
			int sampSplit = numSamples * channels / 2048;
			for (int i = 0; i < 2048; i++)
			{
				table.inputData(0, 0, i, WAVFile.getSample(i*sampSplit));
			}
			table.inputData(0, 0, 2048, table.readData(0, 0, 0));
		}
		else if (importType == 3) // 2048 Split
		{
			constPeriodImport(2048, numSamples * channels, &WAVFile, morphType);
		}
		else if (importType == 4) // FFT to find period NOT WORKING
		{
			int newSize = dmath.nextPowerTwo(numSamples * channels);
			
			newFloat newTable(newSize);
			float sep = (float)(newSize) / (float)(numSamples * channels);
			int lastIndex = 0;
			int currentIndex = 0;
			float ratio;
			for (int s = 1; s < numSamples * channels; s++)
			{
				currentIndex = (int)(s * sep);
				newTable.input(WAVFile.getSample(s), currentIndex);

				for (int i = lastIndex; i < currentIndex; i++)
				{
					ratio = (float)(i - lastIndex) / (float)(currentIndex - lastIndex);
					float value = WAVFile.getSample(s-1) + ratio * WAVFile.getSample(s);
					newTable.input(value, i);
				}
				lastIndex = currentIndex;
			}

			FFT WAVFFT(newSize);
			WAVFFT.fft(newTable.point(), newSize);
			int lowestHarmonic;
			for (int i = 1; i < numSamples * channels; i++)
			{
				float amplitude = (std::sqrt(WAVFFT.getReal(i) * WAVFFT.getReal(i) + WAVFFT.getImag(i) * WAVFFT.getImag(i))) / newSize;
				if (amplitude > 1.0f)
				{
					lowestHarmonic = i;
					i = numSamples * channels;
				}
			}
			int period = newSize / lowestHarmonic;
			constPeriodImport(period, numSamples * channels, &WAVFile, morphType);
		}
		else
		{
			throw std::runtime_error("Import Failed");
			return;
		}

	}
	//============================================================

	void fillBands()
	{
		table.copyFromZero();
		int topBin;
		for(int band = 1; band < BANDS; band++)
		{
			table.fft(band);
			topBin = dmath.getTopBin(band,sampleRate);
			if (topBin == -1)
			{
				throw std::runtime_error("Couldnt perform FFT");
				return;
			}

			table.removeHarmonics(band, topBin, 2048);
			table.ifft(band);
		}
	}
	//============================================================

	void setSampleRate(float sr)
	{
		sampleRate = sr;
	}
	//============================================================

private:
	const float WTsampInv = 1.0f / 2048;
	const float bandOneRange = 23.4604105572f;
	float sampleRate;
	DMath dmath;
	WavetableData table;

	void constPeriodImport(int period, int WAVNumSamples, WAVRead *file, int morphMode)
	{
		int numTables = ((WAVNumSamples) / period);

		float sampleSplit = (float)(period) / 2048.0f;

		int waveDuplicates = (256) / (numTables);

		int leftOverTables = 256 - waveDuplicates * numTables;

		int tableIndex;

		float value;

		int index;

		for (int i = 0; i < numTables; i++)
		{
			tableIndex = period * i;

				for (int s = 0; s < 2048; s++)
				{
					index = tableIndex + (int)(s * sampleSplit);
					if (index < WAVNumSamples)
					{
						value = file->getSample(index);
					}
					table.inputData(0, (i * waveDuplicates), s, value);
				}
				table.inputData(0, (i * waveDuplicates), 2048, table.readData(0, (i * waveDuplicates), 0));

				if (morphMode == 0) // Fill Unused Waves with 0
				{
					for (int w = 1; w < waveDuplicates; w++)
					{
						for (int s = 0; s < 2048; s++)
						{
							table.inputData(0, (i * waveDuplicates) + w, s, 0.0f);
						}
						table.inputData(0, (i * waveDuplicates) + w, 2048, table.readData(0, (i * waveDuplicates) + w, 0));
					}
				}
				else if (morphMode == 1) // Fill Unused Waves with Copies
				{
					for (int w = 1; w < waveDuplicates; w++)
					{
						for (int s = 0; s < 2048; s++)
						{
							table.inputData(0, (i * waveDuplicates) + w, s, table.readData(0, (i * waveDuplicates), s));
						}
						table.inputData(0, (i * waveDuplicates) + w, 2048, table.readData(0, (i * waveDuplicates) + w, 0));
					}
				}
				else if (morphMode == 2) // Linear Interpolation
				{
					if (i > 0)
					{
						interpolateTables((i * waveDuplicates), ((i - 1) * waveDuplicates));
						if (i == numTables - 1)
						{
							tableIndex = period * (i+1);
							for (int s = 0; s < 2048; s++)
							{
								index = tableIndex + (int)(s * sampleSplit);
								if (index < WAVNumSamples)
								{
									value = file->getSample(index);
								}
								table.inputData(0, 255, s, value);
							}
						}
						interpolateTables(255, (i * waveDuplicates));
					}
				}
				else if (morphMode == 3) // Spectral Morph
				{
					if (i > 0)
					{
						spectralMorphTables((i * waveDuplicates), ((i - 1) * waveDuplicates));
						if (i == numTables - 1)
						{
							tableIndex = period * (i + 1);
							for (int s = 0; s < 2048; s++)
							{
								index = tableIndex + (int)(s * sampleSplit);
								if (index < WAVNumSamples)
								{
									value = file->getSample(index);
								}
								table.inputData(0, 255, s, value);
							}
						}
						spectralMorphTables(255, (i * waveDuplicates));
					}
				}
				else
				{
					throw std::runtime_error("Import Failed");
					return;
				}
		}
		
	}
	
	void interpolateTables(int tableA, int tableB)
	{
		int Tdiff = tableB - tableA;
		if (Tdiff < 0)
		{
			int temp = tableB;
			tableB = tableA;
			tableA = temp;
		}
		float diff[2048];
		for (int s = 0; s < 2048; s++)
		{
			diff[s] = table.readData(0, tableB, s) - table.readData(0, tableA, s);
		}

		float ratio;
		for (int w = tableA; w < tableB; w++)
		{
			ratio = (float)(w - tableA) / (float)(tableB - tableA);
			for (int s = 0; s < 2048; s++)
			{
				table.inputData(0, w, s, table.readData(0, tableA, s) + ratio * diff[s]);
			}
			table.inputData(0, w, 2048, table.readData(0, w, 0));
		}
	}
	void spectralMorphTables(int tableA, int tableB)
	{
		table.fft(0);
		int Tdiff = tableB - tableA;
		if (Tdiff < 0)
		{
			int temp = tableB;
			tableB = tableA;
			tableA = temp;
		}

		float diffR[2048];
		for (int s = 0; s < 2048; s++)
		{
			diffR[s] = table.getReal(0, tableB, s) - table.getReal(0, tableA, s);
		}

		float diffI[2048];
		for (int s = 0; s < 2048; s++)
		{
			diffI[s] = table.getImag(0, tableB, s) - table.getImag(0, tableA, s);
		}

		float ratio;
		for (int w = tableA; w < tableB; w++)
		{
			ratio = (float)(w - tableA) / (float)(tableB - tableA);
			for (int s = 0; s < 2048; s++)
			{
				table.setReal(0, w, s, table.getReal(0, tableA, s) + ratio * diffR[s]);
				table.setImag(0, w, s, table.getImag(0, tableA, s) + ratio * diffI[s]);
			}
		}
		table.ifft(0);
	}
};
