/*
  ==============================================================================

	SynthVoice.h
	Created: 11 Oct 2018 10:55:48pm
	Author:  Denver

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Wavetable.h"
#include "DMath.h"
#define BANDS 11

class SynthVoice : public SynthesiserVoice
{
public:
	SynthVoice()
	{
		updateSampleRate();
	}

	bool canPlaySound(SynthesiserSound* sound)
	{
		return dynamic_cast<SynthesiserSound*>(sound) != nullptr;
	}
	//====================================

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition)
	{
		//midiFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		//updateAngleDelta();
		//level = velocity * 0.25f;
	}
	//====================================

	void stopNote(float velocity, bool alowTailOff)
	{
		//level = 0.0f;
		//clearCurrentNote();
	}
	//====================================

	void pitchWheelMoved(int newPitchWheelValue)
	{
		pitchWheelValue = newPitchWheelValue;
	}
	//====================================

	void controllerMoved(int controllerNumber, int newControllerValue)
	{

	}
	//====================================

	void updateSampleRate()
	{
		sampleRate = getSampleRate();
		wT.setSampleRate(sampleRate);
		//wT.fillBands();
	}
	//====================================

	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
	{
		if (level > 0.0f)
		{

			for (int samp = 0; samp < numSamples; samp++)
			{

				//Sweep for time seconds linearly between frequencyHz and topFreqHz

				frequency += (topFreq / (time * sampleRate));
				if (frequency > topFreq)
				{
					frequency = bottomFreq;
				}


				currentBand = 0; //dMath.getBand(frequency);

				//Interpolation
				interpolVal1 = wT.getSampValue(currentBand, wTPos-1, floor(currentPhase));
				interpolVal2 = wT.getSampValue(currentBand, wTPos-1, (floor(currentPhase) + 1));
				interpolRatio = (currentPhase - floor(currentPhase));

				nextSample = level * (interpolVal1 + interpolRatio * (interpolVal2 - interpolVal1));

				//nextSample = (level * wT.getSampValue(currentBand, wTPos, currentPhase));

				for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
				{
					outputBuffer.addSample(channel, startSample + samp, nextSample);
				}

				currentPhase += angleDelta;
				if (currentPhase >= 2048)
				{
					currentPhase -= 2048;
				}
				updateAngleDelta();
			}
		}
	
	}
	//====================================
	void getParam(float* AWTP)
	{
		wTPos = int(*AWTP);
	}
	//====================================
	//Parameterized Variables

	int wTPos = 1;
	float level = 0.01f;
	//====================================

private:
	void updateAngleDelta()
	{
		float cyclesPerSample = frequency / sampleRate;
		angleDelta = cyclesPerSample * 2048;
	}

	Wavetable wT;
	DMath dMath;

	float sampleRate;
	
	double midiFrequency = 0.0;
	float angleDelta = 0.0f;
	float currentPhase = 0.0f;
	float nextSample = 0.0f;
	int pitchWheelValue = 0.0;
	float interpolVal1 = 0.0f;
	float interpolVal2 = 0.0f;
	float bandInterpolVal1 = 0.0f;
	float bandInterpolVal2 = 0.0f;
	float interpolRatio = 0.0f;
	int currentBand = 1;

	double frequency = 200;
	int time = 20;
	double topFreq = 500;
	double bottomFreq = 200;
};