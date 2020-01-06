/*
  ==============================================================================

	WAVRead.h
	Created: 28 Nov 2018 3:04:18pm
	Author:  denver

  ==============================================================================
*/

#pragma once
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include "newFloatArray.h"
#include "newCharArray.h"

using namespace std;

class WAVRead
{
public:
	WAVRead(const char fileName[])
	{
		open(fileName);
	}

	WAVRead()
	{

	}

	void open(const char fileName[])
	{
		char c;
		int index = 0;

		ifstream infile;
		infile.open(fileName, ios::binary | ios::in);

		fileSize = filesize(fileName);
		fileMap.setSize(fileSize);

		for (int i = 0; i < fileSize; i++)
		{
			if (i > 365)
			{
				int breakp = 1;
			}
			infile.read(&c, 1);
			fileMap.input(c, i);
		}

		infile.close();

		//==============================================================

		parseRIFFChunk();
		parseFmtChunk();
		parseDataChunk();
	}

	int getNumSamples()
	{
		return numSamples;
	}

	int getNumChannels()
	{
		return numChannels;
	}

	int getSampleRate()
	{
		return sampleRate;
	}

	float getSample(int i)
	{
		if (i > -1 && i < numSamples * numChannels)
		{
			return buffer.get(i);
		}
		else
		{
			throw std::runtime_error("getSample Failed");
			return NULL;
		}
	}

	float* getBufferPointer()
	{
		return buffer.point();
	}

	void resizeBuffer(int size)
	{
		buffer.setSize(size);
	}
private:

	void parseRIFFChunk()
	{
		if (get4C(0) != 'RIFF')
		{
			throw std::runtime_error("Bad WAV header");
			return;
		}

		wavSize = getU32(4);


		if (get4C(8) != 'WAVE')
		{
			throw std::runtime_error("Bad WAV header");
			return;
		}
		if (get4C(12) == 'JUNK')
		{
			junkSize = getU32(16);
		}
	}

	void parseFmtChunk()
	{
		fmtIndex = 12 + junkSize;

		formatChunkSize = getU32(fmtIndex + 4) + 8;

		formatType = getU16(fmtIndex + 8);

		numChannels = getU16(fmtIndex + 10);

		sampleRate = getU32(fmtIndex + 12);

		avgBytesPerSec = getU32(fmtIndex + 16);

		blockAlign = getU16(fmtIndex + 20);

		bitsPerSample = getU16(fmtIndex + 22);

		bytesPerSample = (bitsPerSample / 8);
	}

	void parseDataChunk()
	{
		int dataIndex = fmtIndex + formatChunkSize + junkSize;
		
		if (get4C(dataIndex) != 'data')
		{
			throw std::runtime_error("Bad WAV header");
			return;
		}

		dataChunkSize = getU32(dataIndex + 4);

		numSamples = (dataChunkSize - 8) / blockAlign;

		buffer.setSize(numSamples * numChannels);

		if (formatType == 1)
		{
			for (int i = 0; i < numSamples * numChannels; i++)
			{
				if (bytesPerSample == 2)
				{
					buffer.input(getI16F(dataIndex + 8 + i * bytesPerSample), i);
				}
				else if (bytesPerSample == 3)
				{
					buffer.input(getI24F(dataIndex + 8 + i * bytesPerSample), i);
				}
				else if (bytesPerSample == 4)
				{
					buffer.input(getI32F(dataIndex + 8 + i * bytesPerSample), i);
				}
				else
				{
					throw std::runtime_error("Unsupported bit depth");
					return;
				}
			}
		}

		else if (formatType == 3)
		{
			for (int i = 0; i < numSamples * numChannels; i++)
			{
				if (bytesPerSample == 4)
				{
					buffer.input(getF32F(dataIndex + 8 + i * bytesPerSample), i);
				}
				else
				{
					throw std::runtime_error("Unsupported bit depth");
					return;
				}
			}
		}

		else
		{
			throw std::runtime_error("Unsupported format Type");
			return;
		}
	}

	//==============================================================

	uint32_t getU32(int nIndex) const
	{
		return fileMap.get(nIndex + 0) | (fileMap.get(nIndex + 1) << 8) | (fileMap.get(nIndex + 2) << 16) | (fileMap.get(nIndex + 3) << 24);
	}

	uint32_t get4C(int nIndex) const
	{
		return fileMap.get(nIndex + 3) | (fileMap.get(nIndex + 2) << 8) | (fileMap.get(nIndex + 1) << 16) | (fileMap.get(nIndex + 0) << 24);
	}

	uint16_t getU16(int nIndex) const
	{
		return fileMap.get(nIndex + 0) | (fileMap.get(nIndex + 1) << 8);
	}

	int32_t getI32(int nIndex) const
	{
		return fileMap.get(nIndex + 0) | (fileMap.get(nIndex + 1) << 8) | (fileMap.get(nIndex + 2) << 16) | (fileMap.get(nIndex + 3) << 24);
	}

	int32_t getI24(int nIndex) const
	{
		int32_t v = fileMap.get(nIndex + 0) | (fileMap.get(nIndex + 1) << 8) | (fileMap.get(nIndex + 2) << 16);
		if (v & 0x800000)
		{
			v |= 0xff000000;
		}
		return v;
	}

	int16_t getI16(int nIndex) const
	{
		return fileMap.get(nIndex + 0) | (fileMap.get(nIndex + 1) << 8);
	}

	float getI16F(int nIndex) const
	{
		return (float)(getI16(nIndex)) / (float)(32767);
	}

	float getI24F(int nIndex) const
	{
		//https://github.com/adamstark/AudioFile/blob/master/AudioFile.cpp

		return (float)getI24(nIndex) / (float)(8388608);
	}

	float getI32F(int nIndex) const
	{
		return (float)(getI32(nIndex)) / (float)(INT_FAST32_MAX);
	}

	float getF32F(int nIndex) const
	{
		union
		{
			int sampI;
			float sampF;
		};

		sampI = getI32(nIndex);
		return sampF;
	}

	//==============================================================

	std::ifstream::pos_type filesize(const char* filename)
	{
		std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
		return in.tellg();
	}

	//==============================================================

	FILE *fp = nullptr;
	newChar fileMap;
	newFloat buffer;

	int fileSize = 0;

	int fmtIndex;
	int dataIndex;

	int numSamples = -1;
	int numChannels = -1;

	uint32_t wavSize;			//Size of file in bytes
	uint32_t junkSize = 0;
	uint32_t formatChunkSize;	//Size of format chunk in bytes
	uint32_t sampleRate;		//Samplerate of file
	uint32_t avgBytesPerSec;	//Number of bytes per second (sampleRate * blockAlign)
	uint32_t dataChunkSize;		//Size of data chunk in bytes

	short formatType;			//Format type. This program will only support 1 and 3
	short blockAlign;			//Number of bytes in a frame (bytes per sample in ALL CHANNELS)			
	short bitsPerSample;		//Bit depth. This program will only support 8, 16, 24, and 32 bit depth
	short bytesPerSample;       //Bit depth. This program will only support 8, 16, 24, and 32 bit depth
};