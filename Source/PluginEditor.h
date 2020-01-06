/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WavetableSynthAudioProcessorEditor : public AudioProcessorEditor, Slider::Listener
{
public:
	WavetableSynthAudioProcessorEditor(WavetableSynthAudioProcessor&); 
	~WavetableSynthAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;


private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.

	void sliderValueChanged(Slider* slider) override;

	Slider AWTPS;
	Slider AVS;

	ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> sliderTree;

	WavetableSynthAudioProcessor& processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableSynthAudioProcessorEditor)
};
