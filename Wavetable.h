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
#include "fft.h"
#define BANDS 11

class Wavetable
{
public:
	Wavetable()
	{
		importWavetable("C:\\Users\\denve\\OneDrive\\Desktop\\Wavetables\\BasicShapes32.wav", 5);
		
		//fillBands();
	}
	//============================================================

	float getSampValue(int band, int waveNumber, int samp)
	{
		return table.readData(band, waveNumber, samp);
	}
	//============================================================

	void importWavetable(char fileName[], int importType)
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

		if (importType == 1) // Split WAV into 256 even segments. Resample each segment to 2048 and input to table.
		{
			int wtSplit = (int)((numSamples * channels) / 256);
			int sampSplit = (int)(wtSplit / 2048);
			for (int w = 0; w < 256; w++)
			{
				for (int s = 0; s < 2048; s++)
				{
					float value = WAVFile.getSample(((w * wtSplit) + (s * sampSplit)));
					table.inputData(0, w, s, table.readData(0, w, s) + value);

				}
				table.inputData(0, w, 2048, table.readData(0, w, 0));
			}
		}
		/*
		else if (importType == 2)// Take FFT of WAV. Split WAV into 256 segments By base frequency.
		{
			int newSize;
			newSize = dmath.nextPowerTwo(numSamples);
			newFloat newTable(newSize);
			for (int i = 0; i < newSize; i++)
			{
				if (i < numSamples)
				{
					newTable.input(WAVFile.getSample(i), i);
				}
				else
				{
					newTable.input(0.0f, i);
				}
			}
			fftObject.fft(newTable.point(), newSize);
			int loudestBin = 1;
			float BinValue = 0.0f;
			for (int i = 0; i < newSize; i++)
			{
				if (newTable.get(i) > BinValue)
				{
					BinValue = newTable.get(i);
					loudestBin = i;
				}
			}
		}
		*/
		else if (importType == 3) // Single Cycle Import
		{
			int sampSplit = numSamples * channels / 2048;
			for (int i = 0; i < 2048; i++)
			{
				table.inputData(0, 0, i, WAVFile.getSample(i*sampSplit));
			}
			table.inputData(0, 0, 2048, table.readData(0, 0, 0));
		}
		else if (importType == 4) // split WAV by first zero crossing
		{
			int zeroCrossIndex = 0;

			for (int i = 2; i < numSamples * channels; i++)
			{
				if ((WAVFile.getSample(i) * WAVFile.getSample(i-1)) <= 0)
				{
					zeroCrossIndex = i;
					i = numSamples * channels;
				}
			}
			
			int period = zeroCrossIndex * 2;

			constPeriodImport(period, numSamples * channels, &WAVFile);

		}
		else if (importType == 5) // 2048 Split
		{
			constPeriodImport(2048, numSamples * channels, &WAVFile);
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
		table.fft(0);
		float newTable[2048];
		int topBin;
		for(int band = 1; band < BANDS; band++)
		{
			topBin = dmath.getTopBin(band,sampleRate);
			if (topBin == -1)
			{
				std::cout << "ERROR" << std::endl;
				throw std::runtime_error("Couldnt perform FFT");
				return;
			}
			for (int w = 0; w < 256; w++)
			{
				for (int bin = 0; bin < 2048; bin++)
				{
					if (bin < topBin)
					{
						newTable[bin] = table.readData(0, w, bin);
					}
					else
					{
						newTable[bin] = 0.0f;
					}
				}
				fftObject.ifft(newTable, 2048);
				for (int s = 0; s < 2048; s++)
				{
					table.inputData(band, w, s, newTable[s]);
				}
				table.inputData(band, w, 2048, table.readData(band, w, 0));
			}
		}
		table.ifft(0);
	}

	void setSampleRate(float sr)
	{
		sampleRate = sr;
	}

private:
	const float WTsampInv = 1.0f / 2048;
	const float bandOneRange = 23.4604105572f;
	float sampleRate;
	DMath dmath;
	FFT fftObject;
	WavetableData table;

	void constPeriodImport(int period, int WAVNumSamples, WAVRead *file)
	{
		int numTables = ((WAVNumSamples) / period) + 1;

		float sampleSplit = (float)(period) / 2048.0f;

		int waveDuplicates = 256 / numTables;

		int leftOverTables = 256 - waveDuplicates * numTables;

		int tableIndex;

		float value;

		int index;

		for (int i = 0; i < numTables; i++)
		{
			tableIndex = period * i;
			for (int w = 0; w < waveDuplicates; w++)
			{
				for (int s = 0; s < 2048; s++)
				{
					index = tableIndex + (int)(s * sampleSplit);
					if (index < WAVNumSamples)
					{
						value = file->getSample(index);
					}
					table.inputData(0, (i * waveDuplicates) + w, s, value);
				}
				table.inputData(0, w, 2048, table.readData(0, w, 0));
			}

		}

		for (int i = 0; i < leftOverTables; i++)
		{
			for (int s = 0; s < 2048; s++)
			{
				table.inputData(0, (256 - leftOverTables + i),s , table.readData(0, 255 - leftOverTables, s));
			}
		}
	}

};
