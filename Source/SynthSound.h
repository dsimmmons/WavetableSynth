/*
  ==============================================================================

	SynthSound.h
	Created: 11 Oct 2018 10:55:58pm
	Author:  Denver

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class SynthSound : public SynthesiserSound
{
public:
	bool appliesToNote(int /*midiNoteNumber*/)
	{
		return true;
	}

	bool appliesToChannel(int /*midiChannel*/)
	{
		return true;
	}
};