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

String getFixedPosSourceName(int index, int dimension) {
    if (dimension == 0)
        return String("S") + String(index + 1) + String("_X");
    else if (dimension == 1)
        return String("S") + String(index + 1) + String("_Y");
    else if (dimension == 2)
        return String("S") + String(index + 1) + String("_Z");
}

// The parameter Layout creates the automatable parameters.
AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    using Parameter = AudioProcessorValueTreeState::Parameter;

    std::vector<std::unique_ptr<Parameter>> parameters;

    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_x"), String("Recording Trajectory X"),
                                                     String(), NormalisableRange<float>(0.f, 1.f), 0.5f, nullptr, nullptr));
    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_y"), String("Recording Trajectory Y"),
                                                     String(), NormalisableRange<float>(0.f, 1.f), 0.5f, nullptr, nullptr));
    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_z"), String("Recording Trajectory Z"),
                                                     String(), NormalisableRange<float>(0.f, 1.f), 0.5f, nullptr, nullptr));

    for (int i = 0; i < MAX_NUMBER_OF_SOURCES; i++) {
        String id(i);
        String id1(i + 1);
        parameters.push_back(std::make_unique<Parameter>(
                                 String("azimuthSpan_") + id, String("Source ") + id1 + String(" Azimuth Span"),
                                 String(), NormalisableRange<float>(0.f, 1.f), 0.f, nullptr, nullptr));
        parameters.push_back(std::make_unique<Parameter>(
                                 String("elevationSpan_") + id, String("Source ") + id1 + String(" Elevation Span"),
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
    parameters (*this, nullptr, Identifier(JucePlugin_Name), createParameterLayout()),
    fixPositionData (FIXED_POSITION_DATA_TAG)
{
    m_lock = false;
    m_somethingChanged = false;
    m_numOfSources = 1;
    m_firstSourceId = 1;
    m_selectedSourceId = 1;
    m_selectedOscFormat = 1;
    m_currentOSCPort = 18032;
    m_lastConnectedOSCPort = -1;
    m_oscConnected = true;

    m_initTimeOnPlay = m_currentTime = 0.0;
    m_lastTime = 10000000.0;

    // Size of the plugin window.
    parameters.state.addChild ({ "uiState", { { "width",  600 }, { "height", 680 } }, {} }, -1, nullptr);

    // Global setting parameters.
    parameters.state.setProperty("oscFormat", 1, nullptr);
    parameters.state.setProperty("oscPortNumber", 18032, nullptr);
    parameters.state.setProperty("oscConnected", true, nullptr);
    parameters.state.setProperty("numberOfSources", 2, nullptr);
    parameters.state.setProperty("firstSourceId", 1, nullptr);
    parameters.state.setProperty("azimuthSpanLink", false, nullptr);
    parameters.state.setProperty("elevationSpanLink", false, nullptr);

    // Per source parameters. Because there is no attachment to the automatable
    // parameters, we need to keep track of the current parameter values to be
    // able to reload the last state of the plugin when we close/open the UI.
    for (int i = 0; i < MAX_NUMBER_OF_SOURCES; i++) {
        String id(i);
        // Non-automatable, per source, parameters.
        parameters.state.setProperty(String("p_azimuth_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_elevation_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_distance_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_x_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_y_") + id, 0.0, nullptr);

        // Automatable, per source, parameters.
        parameters.addParameterListener(String("azimuthSpan_") + id, this);
        parameters.addParameterListener(String("elevationSpan_") + id, this);

        // Gives the source an initial id.
        sources[i].setId(i + m_firstSourceId - 1);
    }

    // Automation values for the recording trajectory.
    parameters.addParameterListener(String("recordingTrajectory_x"), this);
    parameters.addParameterListener(String("recordingTrajectory_y"), this);
    parameters.addParameterListener(String("recordingTrajectory_z"), this);

    automationManager.addListener(this);
    automationManagerAlt.addListener(this);

    // The timer's callback send OSC messages periodically.
    //-----------------------------------------------------
    startTimerHz(60);
}

ControlGrisAudioProcessor::~ControlGrisAudioProcessor() {
    disconnectOSC();
}

//==============================================================================
void ControlGrisAudioProcessor::parameterChanged(const String &parameterID, float newValue) {
    int paramId, sourceId = parameterID.getTrailingIntValue();

    bool needToLinkSourcePositions = false;
    if (parameterID.compare("recordingTrajectory_x") == 0) {
        automationManager.setPlaybackPositionX(newValue);
        needToLinkSourcePositions = true;
    } else if (parameterID.compare("recordingTrajectory_y") == 0) {
        automationManager.setPlaybackPositionY(newValue);
        needToLinkSourcePositions = true;
    } else if (parameterID.compare("recordingTrajectory_z") == 0 && m_selectedOscFormat == SPAT_MODE_LBAP) {
        automationManagerAlt.setPlaybackPositionY(newValue);
        linkSourcePositionsAlt();
    }

    if (needToLinkSourcePositions) {
        linkSourcePositions();
    }

    if (parameterID.startsWith("azimuthSpan_")) {
        paramId = SOURCE_ID_AZIMUTH_SPAN;
    } else if (parameterID.startsWith("elevationSpan_")) {
        paramId = SOURCE_ID_ELEVATION_SPAN;
    } else {
        paramId = -1;
    }

    if (paramId != -1) {
        setSourceParameterValue(sourceId, paramId, newValue);
        m_somethingChanged = true;
    }
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

void ControlGrisAudioProcessor::setSelectedSourceId(int id) {
    m_selectedSourceId = id;
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
        message.clear();
        float azim = -sources[i].getAzimuth() / 180.0 * M_PI;
        float elev = (M_PI / 2.0) - (sources[i].getElevation() / 360.0 * M_PI * 2.0);
        message.addInt32(sources[i].getId());
        message.addFloat32(azim);
        message.addFloat32(elev);
        message.addFloat32(sources[i].getAzimuthSpan() * 2.0);
        message.addFloat32(sources[i].getElevationSpan() * 0.5);
        if (m_selectedOscFormat == 2) {
            message.addFloat32(sources[i].getDistance() / 0.6);
        } else {
            message.addFloat32(sources[i].getDistance());
        }
        message.addFloat32(0.0);

        if (!oscSender.send(message)) {
            std::cout << "Error: could not send OSC message." << std::endl;
            return;
        }
    }
}

void ControlGrisAudioProcessor::timerCallback() {
    sendOscMessage();
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
            parameters.getParameterAsValue("azimuthSpan_" + id).setValue(value);
            break;
        case SOURCE_ID_ELEVATION_SPAN:
            sources[sourceId].setElevationSpan(value);
            parameters.getParameterAsValue("elevationSpan_" + id).setValue(value);
            break;
    }

    setLinkedParameterValue(sourceId, parameterId);
}

// Checks if link buttons are on and update sources consequently.
//---------------------------------------------------------------
void ControlGrisAudioProcessor::setLinkedParameterValue(int sourceId, int parameterId) {
    bool linkAzimuthSpan = (parameterId == SOURCE_ID_AZIMUTH_SPAN && parameters.state.getProperty("azimuthSpanLink", false));
    bool linkElevationSpan = (parameterId == SOURCE_ID_ELEVATION_SPAN && parameters.state.getProperty("elevationSpanLink", false));
    for (int i = 0; i < m_numOfSources; i++) {
        String id(i);
        if (linkAzimuthSpan) {
            sources[i].setAzimuthSpan(sources[sourceId].getAzimuthSpan());
            parameters.getParameterAsValue("azimuthSpan_" + id).setValue(sources[i].getAzimuthSpan());
        }
        if (linkElevationSpan) {
            sources[i].setElevationSpan(sources[sourceId].getElevationSpan());
            parameters.getParameterAsValue("elevationSpan_" + id).setValue(sources[i].getElevationSpan());
        }
    }
}

void ControlGrisAudioProcessor::trajectoryPositionChanged(AutomationManager *manager, Point<float> position) {
    if (manager == &automationManager) {
        parameters.getParameterAsValue("recordingTrajectory_x").setValue(position.x);
        parameters.getParameterAsValue("recordingTrajectory_y").setValue(position.y);
        linkSourcePositions();
    } else if (manager == &automationManagerAlt) {
        parameters.getParameterAsValue("recordingTrajectory_z").setValue(position.y);
        linkSourcePositionsAlt();
    }
}

void ControlGrisAudioProcessor::linkSourcePositions() {
    float deltaAzimuth = 0.0f, deltaX = 0.0f, deltaY = 0.0f;

    switch (automationManager.getSourceLink()) {
        case SOURCE_LINK_INDEPENDANT:
            sources[m_selectedSourceId].setPos(automationManager.getSourcePosition());
            break;
        case SOURCE_LINK_CIRCULAR:
        case SOURCE_LINK_CIRCULAR_FIXED_RADIUS:
        case SOURCE_LINK_CIRCULAR_FIXED_ANGLE:
        case SOURCE_LINK_CIRCULAR_FULLY_FIXED:
            deltaAzimuth = automationManager.getSource().getAzimuth();
            for (int i = 0; i < m_numOfSources; i++) {
                sources[i].setCoordinatesFromFixedSource(deltaAzimuth, 0.0f, 0.0f);
            }
            break;
        case SOURCE_LINK_DELTA_LOCK:
            deltaX = automationManager.getSource().getDeltaX();
            deltaY = automationManager.getSource().getDeltaY();
            for (int i = 0; i < m_numOfSources; i++) {
                sources[i].setXYCoordinatesFromFixedSource(deltaX, deltaY);
            }
            break;
    }
}

void ControlGrisAudioProcessor::linkSourcePositionsAlt() {
    float deltaAzimuth = 0.0f, deltaX = 0.0f, deltaY = 0.0f;

    switch (automationManagerAlt.getSourceLink()) {
        case SOURCE_LINK_ALT_INDEPENDANT:
            sources[m_selectedSourceId].setNormalizedElevation((automationManagerAlt.getSourcePosition().y));
            break;
        case SOURCE_LINK_ALT_FIXED_ELEVATION:
            for (int i = 0; i < m_numOfSources; i++) {
                sources[i].setNormalizedElevation((automationManagerAlt.getSourcePosition().y));
            }
            break;
    }
}

void ControlGrisAudioProcessor::addNewFixedPosition() {
    // Should we replace an item if the new one has the same time as one already saved?
    XmlElement *newData = new XmlElement("ITEM");
    newData->setAttribute("Time", getCurrentTime());
    for (int i = 0; i < MAX_NUMBER_OF_SOURCES; i++) {
        newData->setAttribute(getFixedPosSourceName(i, 0), sources[i].getX());
        newData->setAttribute(getFixedPosSourceName(i, 1), sources[i].getY());
        if (m_selectedOscFormat == SPAT_MODE_LBAP) {
            newData->setAttribute(getFixedPosSourceName(i, 2), sources[i].getNormalizedElevation());
        }
    }
    fixPositionData.addChildElement(newData);
    XmlElementDataSorter sorter("Time", true);
    fixPositionData.sortChildElements(sorter);
}

void ControlGrisAudioProcessor::changeFixedPosition(int row, int column, double value) {
    fixPositionData.getChildElement(row-1)->setAttribute(FIXED_POSITION_DATA_HEADERS[column-1], value);
    XmlElementDataSorter sorter("Time", true);
    fixPositionData.sortChildElements(sorter);
}

void ControlGrisAudioProcessor::deleteFixedPosition(int row, int column) {
    while (m_lock) {}
    fixPositionData.removeChildElement(fixPositionData.getChildElement(row), true);
    XmlElementDataSorter sorter("Time", true);
    fixPositionData.sortChildElements(sorter);
}

void ControlGrisAudioProcessor::newEventConsumed() {
    m_somethingChanged = false;
}

bool ControlGrisAudioProcessor::isSomethingChanged() {
    return m_somethingChanged;
}

double ControlGrisAudioProcessor::getInitTimeOnPlay() {
    return m_initTimeOnPlay;
}

double ControlGrisAudioProcessor::getCurrentTime() {
    return m_currentTime;
}

bool ControlGrisAudioProcessor::getIsPlaying() {
    return m_isPlaying;
}

double ControlGrisAudioProcessor::getBPM() {
    return m_bpm;
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
    m_needInitialization = true;
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

void ControlGrisAudioProcessor::setSourceFixedPosition() {
    float x, y;
    for (int i = 0; i < m_numOfSources; i++) {
        x = currentFixPosition->getDoubleAttribute(getFixedPosSourceName(i, 0));
        y = currentFixPosition->getDoubleAttribute(getFixedPosSourceName(i, 1));
        sources[i].setFixedPosition(x, y);
    }
}

void ControlGrisAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) {
    m_lock = true;
    AudioPlayHead* phead = getPlayHead();
    if (phead != nullptr) {
        AudioPlayHead::CurrentPositionInfo playposinfo;
        phead->getCurrentPosition(playposinfo);
        m_isPlaying = playposinfo.isPlaying;
        m_bpm = playposinfo.bpm;
        if (m_needInitialization) {
            m_initTimeOnPlay = m_currentTime = playposinfo.timeInSeconds;
            m_needInitialization = false;
        } else {
            m_currentTime = playposinfo.timeInSeconds;
        }
    }

    // Check if we need to update the fix positions.
    int numFixPositions = fixPositionData.getNumChildElements();
    if (numFixPositions > 0) {
        if (! currentFixPosition) {
            currentFixPosition = fixPositionData.getFirstChildElement();
        }
        if (m_currentTime < m_lastTime) {
            currentFixPosition = fixPositionData.getFirstChildElement();
            while (currentFixPosition && currentFixPosition->getDoubleAttribute("Time") < m_currentTime) {
                currentFixPosition = currentFixPosition->getNextElement();
            }
            setSourceFixedPosition(); // may need to check "shouldBeFixed".
        } else if (m_currentTime >= m_lastTime) {
            XmlElement *nextElement = currentFixPosition->getNextElement();
            if (nextElement && m_currentTime > nextElement->getDoubleAttribute("Time")) {
                currentFixPosition = nextElement;
                setSourceFixedPosition();
            }
        }
    }
    m_lastTime = m_currentTime;
    m_lock = false;
}

//==============================================================================
bool ControlGrisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ControlGrisAudioProcessor::createEditor()
{
    return new ControlGrisAudioProcessorEditor (*this, parameters, automationManager, automationManagerAlt);
}

//==============================================================================
void ControlGrisAudioProcessor::copyFixedPositionXmlElement(XmlElement *src, XmlElement *dest) {
    if (dest == nullptr)
        dest = new XmlElement(FIXED_POSITION_DATA_TAG);

    forEachXmlChildElement (*src, element) {
        XmlElement *newData = new XmlElement("ITEM");
        newData->setAttribute("Time", element->getDoubleAttribute("Time"));
        for (int i = 0; i < MAX_NUMBER_OF_SOURCES; i++) {
            newData->setAttribute(getFixedPosSourceName(i, 0), element->getDoubleAttribute(getFixedPosSourceName(i, 0)));
            newData->setAttribute(getFixedPosSourceName(i, 1), element->getDoubleAttribute(getFixedPosSourceName(i, 1)));
            newData->setAttribute(getFixedPosSourceName(i, 2), element->getDoubleAttribute(getFixedPosSourceName(i, 2)));
        }
        dest->addChildElement(newData);
    }
}

XmlElement * ControlGrisAudioProcessor::getFixedPositionData() {
    return &fixPositionData;
}

void ControlGrisAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.copyState();

    std::unique_ptr<XmlElement> xmlState (state.createXml());

    if (xmlState.get() != nullptr) {
        if (fixPositionData.getNumChildElements() > 0) {
            // Replace if `fixed position` child already exists.
            XmlElement *childExist = xmlState->getChildByName(FIXED_POSITION_DATA_TAG);
            if (childExist) {
                xmlState->removeChildElement(childExist, true);
            }
            XmlElement *positionData = xmlState->createNewChildElement(FIXED_POSITION_DATA_TAG);
            copyFixedPositionXmlElement(&fixPositionData, positionData);
        }
        copyXmlToBinary (*xmlState, destData);
    }
}

void ControlGrisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        XmlElement *positionData = xmlState->getChildByName(FIXED_POSITION_DATA_TAG);
        if (positionData) {
            fixPositionData.deleteAllChildElements();
            copyFixedPositionXmlElement(positionData, &fixPositionData);
        }
        parameters.replaceState (ValueTree::fromXml (*xmlState));
    }

    if (fixPositionData.getNumChildElements() > 0) {
        currentFixPosition = fixPositionData.getFirstChildElement();
    }

    setPluginState();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ControlGrisAudioProcessor();
}
