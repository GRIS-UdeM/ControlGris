/**************************************************************************
 * Copyright 2018 UdeM - GRIS - Olivier Belanger                          *
 *                                                                        *
 * This file is part of ControlGris, a multi-source spatialization plugin *
 *                                                                        *
 * ControlGris is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Lesser General Public License as         *
 * published by the Free Software Foundation, either version 3 of the     *
 * License, or (at your option) any later version.                        *
 *                                                                        *
 * ControlGris is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU Lesser General Public License for more details.                    *
 *                                                                        *
 * You should have received a copy of the GNU Lesser General Public       *
 * License along with ControlGris.  If not, see                           *
 * <http://www.gnu.org/licenses/>.                                        *
 *************************************************************************/
#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    using Parameter = AudioProcessorValueTreeState::Parameter;

    std::vector<std::unique_ptr<Parameter>> parameters;

    for (int i = 0; i < MaxNumberOfSources; i++) {
        String id(i);
        String id1(i + 1);
        parameters.push_back(std::make_unique<Parameter>(
                                 String("azimuth_") + id, String("Source ") + id1 + String(" Azimuth"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("elevation_") + id, String("Source ") + id1 + String(" Elevation"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("distance_") + id, String("Source ") + id1 + String(" Distance"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("azimuthSpan_") + id, String("Source ") + id1 + String(" Azimuth Span"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("elevationSpan_") + id, String("Source ") + id1 + String(" Elevation Span"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("x_") + id, String("Source ") + id1 + String(" X"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("y_") + id, String("Source ") + id1 + String(" Y"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
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
    m_somethingChanged = false;
    m_numOfSources = 1;
    m_firstSourceId = 1;
    m_selectedOscFormat = 1;
    m_currentOSCPort = 18032;
    m_lastConnectedOSCPort = -1;
    m_oscConnected = false;

    // Size of the plugin window.
    parameters.state.addChild ({ "uiState", { { "width",  900 }, { "height", 500 } }, {} }, -1, nullptr);

    // Global parameters.
    parameters.state.setProperty("oscFormat", 1, nullptr);
    parameters.state.setProperty("oscPortNumber", 18032, nullptr);
    parameters.state.setProperty("oscConnected", true, nullptr);
    parameters.state.setProperty("numberOfSources", 2, nullptr);
    parameters.state.setProperty("firstSourceId", 1, nullptr);
    parameters.state.setProperty("azimuthLink", false, nullptr);
    parameters.state.setProperty("elevationLink", false, nullptr);
    parameters.state.setProperty("distanceLink", false, nullptr);
    parameters.state.setProperty("xLink", false, nullptr);
    parameters.state.setProperty("yLink", false, nullptr);
    parameters.state.setProperty("azimuthSpanLink", false, nullptr);
    parameters.state.setProperty("elevationSpanLink", false, nullptr);

    // Per source parameters. Because there is no attachment to the automatable
    // parameters, we need to keep track of the current parameter values to be
    // able to reload the last state of the plugin when we close/open the UI.
    for (int i = 0; i < MaxNumberOfSources; i++) {
        String id(i);
        parameters.state.setProperty(String("p_azimuth_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_elevation_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_distance_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_x_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_y_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_azimuthSpan_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_elevationSpan_") + id, 0.0, nullptr);

        parameters.addParameterListener(String("azimuth_") + id, this);
        parameters.addParameterListener(String("elevation_") + id, this);
        parameters.addParameterListener(String("distance_") + id, this);
        parameters.addParameterListener(String("azimuthSpan_") + id, this);
        parameters.addParameterListener(String("elevationSpan_") + id, this);
        parameters.addParameterListener(String("x_") + id, this);
        parameters.addParameterListener(String("y_") + id, this);

        // Gives the source an initial id.
        sources[i].setId(i + m_firstSourceId - 1);
    }
}

ControlGrisAudioProcessor::~ControlGrisAudioProcessor() {
    disconnectOSC();
}

//==============================================================================
void ControlGrisAudioProcessor::parameterChanged(const String &parameterID, float newValue) {
    int paramId = 0, sourceId = parameterID.getTrailingIntValue();
    if (parameterID.startsWith("azimuth_")) {
        paramId = SOURCE_ID_AZIMUTH;
    } else if (parameterID.startsWith("elevation_")) {
        paramId = SOURCE_ID_ELEVATION;
    } else if (parameterID.startsWith("distance_")) {
        paramId = SOURCE_ID_DISTANCE;
    } else if (parameterID.startsWith("x_")) {
        paramId = SOURCE_ID_X;
    } else if (parameterID.startsWith("y_")) {
        paramId = SOURCE_ID_Y;
    } else if (parameterID.startsWith("azimuthSpan_")) {
        paramId = SOURCE_ID_AZIMUTH_SPAN;
    } else if (parameterID.startsWith("elevationSpan_")) {
        paramId = SOURCE_ID_ELEVATION_SPAN;
    }

    setSourceParameterValue(sourceId, paramId, newValue);

    sendOscMessage();

    m_somethingChanged = true;
}

//==============================================================================
void ControlGrisAudioProcessor::setOscFormat(int oscFormat) {
    m_selectedOscFormat = oscFormat;
    parameters.state.setProperty("oscFormat", m_selectedOscFormat, nullptr);
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setRadiusIsElevation(m_selectedOscFormat != 2);
    }
}

int ControlGrisAudioProcessor::getOscFormat() {
    return m_selectedOscFormat;
}

void ControlGrisAudioProcessor::setOscPortNumber(int oscPortNumber) {
    m_currentOSCPort = oscPortNumber;
    parameters.state.setProperty("oscPortNumber", m_currentOSCPort, nullptr);
}

int ControlGrisAudioProcessor::getOscPortNumber() {
    return m_currentOSCPort;
}

void ControlGrisAudioProcessor::setFirstSourceId(int firstSourceId, bool propagate) {
    m_firstSourceId = firstSourceId;
    parameters.state.setProperty("firstSourceId", m_firstSourceId, nullptr);
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setId(i + m_firstSourceId - 1);
    }

    if (propagate)
        sendOscMessage();
}

int ControlGrisAudioProcessor::getFirstSourceId() {
    return m_firstSourceId;
}

void ControlGrisAudioProcessor::setNumberOfSources(int numOfSources, bool propagate) {
    m_numOfSources = numOfSources;
    parameters.state.setProperty("numberOfSources", m_numOfSources, nullptr);

    if (propagate)
        sendOscMessage();
}

int ControlGrisAudioProcessor::getNumberOfSources() {
    return m_numOfSources;
}

Source * ControlGrisAudioProcessor::getSources() {
    return sources;
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscConnection(int oscPort) {
    disconnectOSC();

    m_oscConnected = oscSender.connect("127.0.0.1", oscPort);
    if (!m_oscConnected)
        std::cout << "Error: could not connect to UDP port " << oscPort << "." << std::endl;
    else
        m_lastConnectedOSCPort = oscPort;

    return m_oscConnected;
}

bool ControlGrisAudioProcessor::disconnectOSC() {
    if (m_oscConnected) {
        if (oscSender.disconnect()) {
            m_oscConnected = false;
            m_lastConnectedOSCPort = -1;
        }
    }
    return !m_oscConnected;
}

bool ControlGrisAudioProcessor::getOscConnected() {
    return m_oscConnected;
}

void ControlGrisAudioProcessor::handleOscConnection(bool state) {
    if (state) {
        if (m_lastConnectedOSCPort != m_currentOSCPort) {
            createOscConnection(m_currentOSCPort);
        }
    } else {
        disconnectOSC();
    }
    parameters.state.setProperty("oscConnected", getOscConnected(), nullptr);
}

void ControlGrisAudioProcessor::sendOscMessage() {
    if (! m_oscConnected)
        return;

    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    for (int i = 0; i < m_numOfSources; i++) {
        if (sources[i].getChanged()) {
            message.clear();
            float azim = -sources[i].getAzimuth() / 180.0 * M_PI;
            float elev = (M_PI / 2.0) - (sources[i].getElevation() / 360.0 * M_PI * 2.0);
            message.addInt32(sources[i].getId());
            message.addFloat32(azim);
            message.addFloat32(elev);
            message.addFloat32(sources[i].getAzimuthSpan() * 2.0);
            message.addFloat32(sources[i].getElevationSpan() * 0.5);
            message.addFloat32(sources[i].getDistance());
            message.addFloat32(0.0);

            if (!oscSender.send(message)) {
                std::cout << "Error: could not send OSC message." << std::endl;
                return;
            }
            sources[i].setChanged(false);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::setPluginState() {
    // Set global settings values.
    //----------------------------
    setOscFormat(parameters.state.getProperty("oscFormat", 1));
    setOscPortNumber(parameters.state.getProperty("oscPortNumber", 18032));
    handleOscConnection(parameters.state.getProperty("oscConnected", true));
    setNumberOfSources(parameters.state.getProperty("numberOfSources", 1), false);
    setFirstSourceId(parameters.state.getProperty("firstSourceId", 1));

    // Set parameter values for sources.
    //----------------------------------
    for (int i = 0; i < m_numOfSources; i++) {
        String id(i);
        sources[i].setNormalizedAzimuth(parameters.state.getProperty(String("p_azimuth_") + id));
        sources[i].setNormalizedElevation(parameters.state.getProperty(String("p_elevation_") + id));
        sources[i].setDistance(parameters.state.getProperty(String("p_distance_") + id));
        sources[i].setAzimuthSpan(parameters.state.getProperty(String("p_azimuthSpan_") + id));
        sources[i].setElevationSpan(parameters.state.getProperty(String("p_elevationSpan_") + id));
    }

    ControlGrisAudioProcessorEditor *editor = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
    if (editor != nullptr) {
        editor->setPluginState();
    }

    sendOscMessage();
}

// Called whenever a source has changed.
//--------------------------------------
void ControlGrisAudioProcessor::setSourceParameterValue(int sourceId, int parameterId, double value) {
    String id(sourceId);
    switch (parameterId) {
        case SOURCE_ID_AZIMUTH:
            sources[sourceId].setNormalizedAzimuth(value);
            parameters.state.setProperty("p_azimuth_" + id, value, nullptr);
            break;
        case SOURCE_ID_ELEVATION:
            sources[sourceId].setNormalizedElevation(value);
            parameters.state.setProperty(String("p_elevation_") + id, value, nullptr);
            break;
        case SOURCE_ID_DISTANCE:
            sources[sourceId].setDistance(value);
            parameters.state.setProperty(String("p_distance_") + id, value, nullptr);
            break;
        case SOURCE_ID_X:
            sources[sourceId].setX(value);
            break;
        case SOURCE_ID_Y:
            sources[sourceId].setY(value);
            break;
        case SOURCE_ID_AZIMUTH_SPAN:
            sources[sourceId].setAzimuthSpan(value);
            parameters.state.setProperty(String("p_azimuthSpan_") + id, value, nullptr);
            break;
        case SOURCE_ID_ELEVATION_SPAN:
            sources[sourceId].setElevationSpan(value);
            parameters.state.setProperty(String("p_elevationSpan_") + id, value, nullptr);
            break;
    }

    sendOscMessage();

    setLinkedParameterValue(sourceId, parameterId);
}

// Checks if link buttons are on and update sources consequently.
//---------------------------------------------------------------
void ControlGrisAudioProcessor::setLinkedParameterValue(int sourceId, int parameterId) {
    String id(sourceId);
    if (parameterId == -1) {
        parameters.state.setProperty("p_azimuth_" + id, sources[sourceId].getNormalizedAzimuth(), nullptr);
        parameters.state.setProperty("p_elevation_" + id, sources[sourceId].getNormalizedElevation(), nullptr);
        parameters.state.setProperty("p_distance_" + id, sources[sourceId].getDistance(), nullptr);
    }

    bool linkAzimuth = false, linkElevation = false, linkDistance = false, linkX = false, linkY = false;
    bool linkAzimuthSpan = (parameterId == SOURCE_ID_AZIMUTH_SPAN && parameters.state.getProperty("azimuthSpanLink", false));
    bool linkElevationSpan = (parameterId == SOURCE_ID_ELEVATION_SPAN && parameters.state.getProperty("elevationSpanLink", false));
    if (parameterId < SOURCE_ID_AZIMUTH) {
        // Source changed from 2D field view.
        linkAzimuth = parameters.state.getProperty("azimuthLink", false);
        linkElevation = parameters.state.getProperty("elevationLink", false);
        linkDistance = parameters.state.getProperty("distanceLink", false);
        linkX = parameters.state.getProperty("xLink", false);
        linkY = parameters.state.getProperty("yLink", false);
    } else if (parameterId < SOURCE_ID_X) {
        // Source changed from polar coordinates.
        linkAzimuth = (parameterId == SOURCE_ID_AZIMUTH && parameters.state.getProperty("azimuthLink", false));
        linkElevation = (parameterId == SOURCE_ID_ELEVATION && parameters.state.getProperty("elevationLink", false));
        linkDistance = (parameterId == SOURCE_ID_DISTANCE && parameters.state.getProperty("distanceLink", false));
        linkX = parameters.state.getProperty("xLink", false);
        linkY = parameters.state.getProperty("yLink", false);
    } else if (parameterId < SOURCE_ID_AZIMUTH_SPAN) {
        // Source changed from cartesian coordinates.
        linkX = (parameterId == SOURCE_ID_X && parameters.state.getProperty("xLink", false));
        linkY = (parameterId == SOURCE_ID_Y && parameters.state.getProperty("yLink", false));
        linkAzimuth = parameters.state.getProperty("azimuthLink", false);
        linkElevation = parameters.state.getProperty("elevationLink", false);
        linkDistance = parameters.state.getProperty("distanceLink", false);
    }
    for (int i = 0; i < m_numOfSources; i++) {
        String id(i);
        if (linkAzimuth) {
            sources[i].setAzimuth(sources[sourceId].getAzimuth());
            parameters.state.setProperty("p_azimuth_" + id, sources[i].getNormalizedAzimuth(), nullptr);
        }
        if (linkElevation) {
            sources[i].setElevation(sources[sourceId].getElevation());
            parameters.state.setProperty(String("p_elevation_") + id, sources[i].getNormalizedElevation(), nullptr);
        }
        if (linkDistance) {
            sources[i].setDistance(sources[sourceId].getDistance());
            parameters.state.setProperty(String("p_distance_") + id, sources[i].getDistance(), nullptr);
        }
        if (linkX) {
            sources[i].setX(sources[sourceId].getX());
        }
        if (linkY) {
            sources[i].setY(sources[sourceId].getY());
        }
        if (linkAzimuthSpan) {
            sources[i].setAzimuthSpan(sources[sourceId].getAzimuthSpan());
            parameters.state.setProperty(String("p_azimuthSpan_") + id, sources[i].getAzimuthSpan(), nullptr);
        }
        if (linkElevationSpan) {
            sources[i].setElevationSpan(sources[sourceId].getElevationSpan());
            parameters.state.setProperty(String("p_elevationSpan_") + id, sources[i].getElevationSpan(), nullptr);
        }
    }

    sendOscMessage();
}

void ControlGrisAudioProcessor::newEventConsumed() {
    m_somethingChanged = false;
}

bool ControlGrisAudioProcessor::isSomethingChanged() {
    return m_somethingChanged;
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
    auto state = parameters.copyState();

    std::unique_ptr<XmlElement> xmlState (state.createXml());

    if (xmlState.get() != nullptr)
        copyXmlToBinary (*xmlState, destData);
}

void ControlGrisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        parameters.replaceState (ValueTree::fromXml (*xmlState));

    setPluginState();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ControlGrisAudioProcessor();
}
