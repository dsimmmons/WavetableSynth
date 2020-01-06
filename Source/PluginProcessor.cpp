/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Wavetable.h"
#include <iostream>

using namespace std;

//==============================================================================
WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
	AWTPP(1),
	tree (*this, nullptr)
#endif
{
	NormalisableRange<float> AWTPParam(1,256);
	tree.createAndAddParameter("awtp", "AWTP", "AWTP", AWTPParam, 1, nullptr, nullptr);
	tree.state = ValueTree("Foo");
	//==============================================================================
	//OSC

	OSCA.clearVoices();

	for (int i = 0; i < 1; i++)
	{
		OSCA.addVoice(new SynthVoice());
	}

	OSCA.clearSounds();
	OSCA.addSound(new SynthSound());

	//==============================================================================
	//Parameters

	
	//==============================================================================
}
WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor()
{
}

//==============================================================================
const String WavetableSynthAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool WavetableSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool WavetableSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool WavetableSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double WavetableSynthAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int WavetableSynthAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int WavetableSynthAudioProcessor::getCurrentProgram()
{
	return 0;
}

void WavetableSynthAudioProcessor::setCurrentProgram(int index)
{
}

const String WavetableSynthAudioProcessor::getProgramName(int index)
{
	return {};
}

void WavetableSynthAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void WavetableSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	ignoreUnused(samplesPerBlock);
	currentSampleRate = sampleRate;
	OSCA.setCurrentPlaybackSampleRate(currentSampleRate);
}

void WavetableSynthAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavetableSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void WavetableSynthAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	const int totalNumInputChannels = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();

	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	for (int i = 0; i < OSCA.getNumVoices(); i++)
	{
		if (OSCAVoice = dynamic_cast<SynthVoice*>(OSCA.getVoice(i)))
		{
			OSCAVoice->getParam(tree.getRawParameterValue("awtp"));
		}
	}

	OSCA.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool WavetableSynthAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{
	return new WavetableSynthAudioProcessorEditor(*this);
}

//==============================================================================
void WavetableSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void WavetableSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WavetableSynthAudioProcessor();
}
