/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    using Parameter = AudioProcessorValueTreeState::Parameter;

    std::vector<std::unique_ptr<Parameter>> parameters;

    for (int i = 0; i < MaxNumOfSources; i++) {
        parameters.push_back(std::make_unique<Parameter>(
                                 String("azimuth_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Azimuth"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("elevation_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Elevation"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("distance_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Distance"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("azimuthSpan_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Azimuth Span"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("elevationSpan_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Elevation Span"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("x_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" X"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("y_") + String(i+1),
                                 String("Source ") + String(i+1) + String(" Y"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f,
                                 nullptr, nullptr, false, false));
    }

    return { parameters.begin(), parameters.end() };
}

//==============================================================================
ControlGrisAudioProcessor::ControlGrisAudioProcessor()
     :
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters (*this, nullptr, Identifier(JucePlugin_Name), createParameterLayout())
{

        // Add a sub-tree to store the state of our UI
        parameters.state.addChild ({ "uiState", { { "width",  900 }, { "height", 500 } }, {} }, -1, nullptr);

    if (! oscSender.connect("127.0.0.1", 18032)) {
        std::cout << "Error: could not connect to UDP port 18032." << std::endl;
    }
}

ControlGrisAudioProcessor::~ControlGrisAudioProcessor()
{
    oscSender.disconnect();
}

//==============================================================================
const String ControlGrisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ControlGrisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ControlGrisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ControlGrisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ControlGrisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ControlGrisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ControlGrisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ControlGrisAudioProcessor::setCurrentProgram (int index)
{
}

const String ControlGrisAudioProcessor::getProgramName (int index)
{
    return {};
}

void ControlGrisAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ControlGrisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ControlGrisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ControlGrisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
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

void ControlGrisAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
}

//==============================================================================
bool ControlGrisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ControlGrisAudioProcessor::createEditor()
{
    return new ControlGrisAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ControlGrisAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ControlGrisAudioProcessorEditor *editor = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());

    auto state = parameters.copyState();

    if (editor != nullptr) {
        state.setProperty("azimuthLink", editor->getParameterLinkState(SOURCE_ID_AZIMUTH), nullptr);
        state.setProperty("elevationLink", editor->getParameterLinkState(SOURCE_ID_ELEVATION), nullptr);
        state.setProperty("distanceLink", editor->getParameterLinkState(SOURCE_ID_DISTANCE), nullptr);
        state.setProperty("xLink", editor->getParameterLinkState(SOURCE_ID_X), nullptr);
        state.setProperty("yLink", editor->getParameterLinkState(SOURCE_ID_Y), nullptr);
        state.setProperty("azimuthSpanLink", editor->getParameterLinkState(SOURCE_ID_AZIMUTH_SPAN), nullptr);
        state.setProperty("elevationSpanLink", editor->getParameterLinkState(SOURCE_ID_ELEVATION_SPAN), nullptr);
    }
    std::unique_ptr<XmlElement> xmlState (state.createXml());

    if (xmlState.get() != nullptr)
        copyXmlToBinary (*xmlState, destData);

}

void ControlGrisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ControlGrisAudioProcessorEditor *editor = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());

    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        parameters.replaceState (ValueTree::fromXml (*xmlState));

    if (editor != nullptr) {
        editor->setParameterLinkState(SOURCE_ID_AZIMUTH, parameters.state.getProperty("azimuthLink", false));
        editor->setParameterLinkState(SOURCE_ID_ELEVATION, parameters.state.getProperty("elevationLink", false));
        editor->setParameterLinkState(SOURCE_ID_DISTANCE, parameters.state.getProperty("distanceLink", false));
        editor->setParameterLinkState(SOURCE_ID_X, parameters.state.getProperty("xLink", false));
        editor->setParameterLinkState(SOURCE_ID_Y, parameters.state.getProperty("yLink", false));
        editor->setParameterLinkState(SOURCE_ID_AZIMUTH_SPAN, parameters.state.getProperty("azimuthSpanLink", false));
        editor->setParameterLinkState(SOURCE_ID_ELEVATION_SPAN, parameters.state.getProperty("elevationSpanLink", false));
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ControlGrisAudioProcessor();
}
