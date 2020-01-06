/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor(WavetableSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{

	getLookAndFeel().setColour(Slider::trackColourId, Colours::red);

	getLookAndFeel().setColour(Slider::thumbColourId, Colours::red);

	setSize(400, 300);

	AWTPS.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	AWTPS.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
	AWTPS.setRange(1, 256);
	AWTPS.setValue(1);
	AWTPS.addListener(this);
	addAndMakeVisible(AWTPS);

	sliderTree = new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "awtp", AWTPS);

	addAndMakeVisible(AVS);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{
}



//==============================================================================
void WavetableSynthAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(Colours::black);

	g.setColour(Colours::red);


}

void WavetableSynthAudioProcessorEditor::resized()
{
	AWTPS.setBounds(getLocalBounds());
}

void WavetableSynthAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &AWTPS)
	{
		processor.AWTPP = AWTPS.getValue();
	}
}
