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

//==============================================================================
ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor& p,
                                                                   AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{
    setLookAndFeel(&mGrisFeel);
 
    m_numOfSources = 1;
    m_selectedSource = 0;
    m_firstSourceId = 1;
    m_selectedOscFormat = 1;

    // Set up the interface.
    //----------------------
    mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&mainBanner);

    elevationBanner.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationBanner);

    parametersBanner.setText("Source Parameters", NotificationType::dontSendNotification);
    addAndMakeVisible(&parametersBanner);

    trajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryBanner);

    settingsBanner.setText("Configuration", NotificationType::dontSendNotification);
    addAndMakeVisible(&settingsBanner);

    mainField.addListener(this);
    addAndMakeVisible(&mainField);

    elevationField.addListener(this);
    addAndMakeVisible(&elevationField);

    parametersBox.addListener(this);
    addAndMakeVisible(&parametersBox);

    addAndMakeVisible(&trajectoryBox);

    settingsBox.addListener(this);

    Colour bg = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    configurationComponent.addTab ("Settings", bg, &settingsBox, false);
    configurationComponent.addTab ("Source", bg, &sourceBox, false);
    configurationComponent.addTab ("Controllers", bg, &interfaceBox, false);
    addAndMakeVisible(configurationComponent);

    // Add sources to the field.
    //--------------------------
    mainField.setSources(sources, m_numOfSources);
    elevationField.setSources(sources, m_numOfSources);

    // Initialize sources and add ParameterListener(s).
    //-------------------------------------------------
    for (int i = 0; i < MaxNumOfSources; i++) {
        String id(i);
        sources[i].setId(i + m_firstSourceId - 1);
        valueTreeState.addParameterListener(String("azimuth_") + id, this);
        valueTreeState.addParameterListener(String("elevation_") + id, this);
        valueTreeState.addParameterListener(String("distance_") + id, this);
        valueTreeState.addParameterListener(String("azimuthSpan_") + id, this);
        valueTreeState.addParameterListener(String("elevationSpan_") + id, this);
        valueTreeState.addParameterListener(String("x_") + id, this);
        valueTreeState.addParameterListener(String("y_") + id, this);
    }

    parametersBox.setSelectedSource(&sources[m_selectedSource]);

    // Manage dynamic window size of the plugin.
    //------------------------------------------
    setResizeLimits(300, 320, 1800, 1300);

    lastUIWidth .referTo (processor.parameters.state.getChildWithName ("uiState").getPropertyAsValue ("width",  nullptr));
    lastUIHeight.referTo (processor.parameters.state.getChildWithName ("uiState").getPropertyAsValue ("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);

    // Load the last saved state of the plugin.
    //-----------------------------------------
    setPluginState();
}

ControlGrisAudioProcessorEditor::~ControlGrisAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

Source * ControlGrisAudioProcessorEditor::getSources() {
    return sources;
}

int ControlGrisAudioProcessorEditor::getSelectedSource() {
    return m_selectedSource;
}

void ControlGrisAudioProcessorEditor::setPluginState() {
    // Set state for the link buttons.
    //--------------------------------
    parametersBox.setLinkState(SOURCE_ID_AZIMUTH, valueTreeState.state.getProperty("azimuthLink", false));
    parametersBox.setLinkState(SOURCE_ID_ELEVATION, valueTreeState.state.getProperty("elevationLink", false));
    parametersBox.setLinkState(SOURCE_ID_DISTANCE, valueTreeState.state.getProperty("distanceLink", false));
    parametersBox.setLinkState(SOURCE_ID_X, valueTreeState.state.getProperty("xLink", false));
    parametersBox.setLinkState(SOURCE_ID_Y, valueTreeState.state.getProperty("yLink", false));
    parametersBox.setLinkState(SOURCE_ID_AZIMUTH_SPAN, valueTreeState.state.getProperty("azimuthSpanLink", false));
    parametersBox.setLinkState(SOURCE_ID_ELEVATION_SPAN, valueTreeState.state.getProperty("elevationSpanLink", false));

    // Set global settings values.
    //----------------------------
    numberOfSourcesChanged(valueTreeState.state.getProperty("numberOfSources", 1));
    firstSourceIdChanged(valueTreeState.state.getProperty("firstSourceId", 1));

    // Set parameter values for sources.
    //----------------------------------
    for (int i = 0; i < m_numOfSources; i++) {
        String id(i);
        sources[i].setNormalizedAzimuth(valueTreeState.state.getProperty(String("p_azimuth_") + id));
        sources[i].setNormalizedElevation(valueTreeState.state.getProperty(String("p_elevation_") + id));
        sources[i].setDistance(valueTreeState.state.getProperty(String("p_distance_") + id));
        sources[i].setAzimuthSpan(valueTreeState.state.getProperty(String("p_azimuthSpan_") + id));
        sources[i].setElevationSpan(valueTreeState.state.getProperty(String("p_elevationSpan_") + id));
    }

    // Update the interface.
    //----------------------
    parametersBox.setSelectedSource(&sources[m_selectedSource]);
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);

    sendOscMessage();
}

// Value::Listener callback. Called when the stored window size changes.
//----------------------------------------------------------------------
void ControlGrisAudioProcessorEditor::valueChanged (Value&) {
    setSize (lastUIWidth.getValue(), lastUIHeight.getValue());
}

// SettingsBoxComponent::Listener callbacks.
//------------------------------------------
void ControlGrisAudioProcessorEditor::oscFormatChanged(int selectedId) {
    m_selectedOscFormat = selectedId;
    bool selectionIsLBAP = m_selectedOscFormat == 2;
    parametersBox.setDistanceEnabled(selectionIsLBAP);
    mainField.setDrawElevation(!selectionIsLBAP);
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setRadiusIsElevation(!selectionIsLBAP);
    }
    resized();
}

void ControlGrisAudioProcessorEditor::numberOfSourcesChanged(int numOfSources) {
    m_selectedSource = 0;
    m_numOfSources = numOfSources;
    settingsBox.setNumberOfSources(m_numOfSources);

    parametersBox.setSelectedSource(&sources[m_selectedSource]);

    mainField.setSources(sources, m_numOfSources);
    elevationField.setSources(sources, m_numOfSources);

    valueTreeState.state.setProperty("numberOfSources", numOfSources, nullptr);

    sendOscMessage();
}

void ControlGrisAudioProcessorEditor::firstSourceIdChanged(int firstSourceId) {
    m_firstSourceId = firstSourceId;
    settingsBox.setFirstSourceId(m_firstSourceId);
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setId(i + m_firstSourceId - 1);
    }

    parametersBox.setSelectedSource(&sources[m_selectedSource]);

    valueTreeState.state.setProperty("firstSourceId", firstSourceId, nullptr);

    mainField.repaint();
    if (m_selectedOscFormat == 2)
        elevationField.repaint();

    sendOscMessage();
}

// ParametersBoxComponent::Listener callbacks.
//--------------------------------------------
void ControlGrisAudioProcessorEditor::parameterLinkChanged(int parameterId, bool value) {
    StringArray parameterNames({"azimuthLink", "elevationLink", "distanceLink", "xLink",
                                "yLink", "azimuthSpanLink", "elevationSpanLink"});
    valueTreeState.state.setProperty(parameterNames[parameterId], value, nullptr);
}

void ControlGrisAudioProcessorEditor::parameterChanged(int parameterId, double value) {
    setSourceParameterValue(m_selectedSource, parameterId, value);

    mainField.repaint();
    if (m_selectedOscFormat == 2)
        elevationField.repaint();

    sendOscMessage();
}

void ControlGrisAudioProcessorEditor::selectedSourceClicked() {
    m_selectedSource = (m_selectedSource + 1) % m_numOfSources;
    parametersBox.setSelectedSource(&sources[m_selectedSource]);
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
}

// AudioProcessorValueTreeState::Listener callback.
//-------------------------------------------------
void ControlGrisAudioProcessorEditor::parameterChanged(const String &parameterID, float newValue) {
    int paramId, sourceId = parameterID.getTrailingIntValue();
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

    if (sourceId == m_selectedSource) {
        parametersBox.setSelectedSource(&sources[sourceId]);
    }

    mainField.repaint();
    if (m_selectedOscFormat == 2)
        elevationField.repaint();

    sendOscMessage();
}

// FieldComponent::Listener callback.
//-----------------------------------
void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {
    m_selectedSource = sourceId;
    parametersBox.setSelectedSource(&sources[sourceId]);
    setLinkedParameterValue(sourceId, -1);

    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);

    sendOscMessage();
}

// Called whenever a source has changed.
//--------------------------------------
void ControlGrisAudioProcessorEditor::setSourceParameterValue(int sourceId, int parameterId, double value) {
    String id(sourceId);
    switch (parameterId) {
        case SOURCE_ID_AZIMUTH:
            sources[sourceId].setNormalizedAzimuth(value);
            valueTreeState.state.setProperty("p_azimuth_" + id, value, nullptr);
            break;
        case SOURCE_ID_ELEVATION:
            sources[sourceId].setNormalizedElevation(value);
            valueTreeState.state.setProperty(String("p_elevation_") + id, value, nullptr);
            break;
        case SOURCE_ID_DISTANCE:
            sources[sourceId].setDistance(value);
            valueTreeState.state.setProperty(String("p_distance_") + id, value, nullptr);
            break;
        case SOURCE_ID_X:
            sources[sourceId].setX(value);
            break;
        case SOURCE_ID_Y:
            sources[sourceId].setY(value);
            break;
        case SOURCE_ID_AZIMUTH_SPAN:
            sources[sourceId].setAzimuthSpan(value);
            valueTreeState.state.setProperty(String("p_azimuthSpan_") + id, value, nullptr);
            break;
        case SOURCE_ID_ELEVATION_SPAN:
            sources[sourceId].setElevationSpan(value);
            valueTreeState.state.setProperty(String("p_elevationSpan_") + id, value, nullptr);
            break;
    }
    setLinkedParameterValue(sourceId, parameterId);
}

// Checks if link buttons are on and update sources consequently.
//---------------------------------------------------------------
void ControlGrisAudioProcessorEditor::setLinkedParameterValue(int sourceId, int parameterId) {
    String id(sourceId);
    if (parameterId == -1) {
        valueTreeState.state.setProperty("p_azimuth_" + id, sources[sourceId].getNormalizedAzimuth(), nullptr);
        valueTreeState.state.setProperty("p_elevation_" + id, sources[sourceId].getNormalizedElevation(), nullptr);
        valueTreeState.state.setProperty("p_distance_" + id, sources[sourceId].getDistance(), nullptr);
    }

    bool linkAzimuth = false, linkElevation = false, linkDistance = false, linkX = false, linkY = false;
    bool linkAzimuthSpan = (parameterId == SOURCE_ID_AZIMUTH_SPAN && parametersBox.getLinkState(SOURCE_ID_AZIMUTH_SPAN));
    bool linkElevationSpan = (parameterId == SOURCE_ID_ELEVATION_SPAN && parametersBox.getLinkState(SOURCE_ID_ELEVATION_SPAN));
    if (parameterId < SOURCE_ID_AZIMUTH) {
        // Source changed from 2D field view.
        linkAzimuth = parametersBox.getLinkState(SOURCE_ID_AZIMUTH);
        linkElevation = parametersBox.getLinkState(SOURCE_ID_ELEVATION);
        linkDistance = parametersBox.getLinkState(SOURCE_ID_DISTANCE);
        linkX = parametersBox.getLinkState(SOURCE_ID_X);
        linkY = parametersBox.getLinkState(SOURCE_ID_Y);
    } else if (parameterId < SOURCE_ID_X) {
        // Source changed from polar coordinates.
        linkAzimuth = (parameterId == SOURCE_ID_AZIMUTH && parametersBox.getLinkState(SOURCE_ID_AZIMUTH));
        linkElevation = (parameterId == SOURCE_ID_ELEVATION && parametersBox.getLinkState(SOURCE_ID_ELEVATION));
        linkDistance = (parameterId == SOURCE_ID_DISTANCE && parametersBox.getLinkState(SOURCE_ID_DISTANCE));
        linkX = (parametersBox.getLinkState(SOURCE_ID_X));
        linkY = (parametersBox.getLinkState(SOURCE_ID_Y));
    } else if (parameterId < SOURCE_ID_AZIMUTH_SPAN) {
        // Source changed from cartesian coordinates.
        linkX = (parameterId == SOURCE_ID_X && parametersBox.getLinkState(SOURCE_ID_X));
        linkY = (parameterId == SOURCE_ID_Y && parametersBox.getLinkState(SOURCE_ID_Y));
        linkAzimuth = (parametersBox.getLinkState(SOURCE_ID_AZIMUTH));
        linkElevation = (parametersBox.getLinkState(SOURCE_ID_ELEVATION));
        linkDistance = (parametersBox.getLinkState(SOURCE_ID_DISTANCE));
    }
    for (int i = 0; i < m_numOfSources; i++) {
        String id(i);
        if (linkAzimuth) {
            sources[i].setAzimuth(sources[sourceId].getAzimuth());
            valueTreeState.state.setProperty("p_azimuth_" + id, sources[i].getNormalizedAzimuth(), nullptr);
        }
        if (linkElevation) {
            sources[i].setElevation(sources[sourceId].getElevation());
            valueTreeState.state.setProperty(String("p_elevation_") + id, sources[i].getNormalizedElevation(), nullptr);
        }
        if (linkDistance) {
            sources[i].setDistance(sources[sourceId].getDistance());
            valueTreeState.state.setProperty(String("p_distance_") + id, sources[i].getDistance(), nullptr);
        }
        if (linkX) {
            sources[i].setX(sources[sourceId].getX());
        }
        if (linkY) {
            sources[i].setY(sources[sourceId].getY());
        }
        if (linkAzimuthSpan) {
            sources[i].setAzimuthSpan(sources[sourceId].getAzimuthSpan());
            valueTreeState.state.setProperty(String("p_azimuthSpan_") + id, sources[i].getAzimuthSpan(), nullptr);
        }
        if (linkElevationSpan) {
            sources[i].setElevationSpan(sources[sourceId].getElevationSpan());
            valueTreeState.state.setProperty(String("p_elevationSpan_") + id, sources[i].getElevationSpan(), nullptr);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::sendOscMessage() {
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

            if (!processor.oscSender.send(message)) {
                std::cout << "Error: could not send OSC message." << std::endl;
                return;
            }
            sources[i].setChanged(false);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::paint (Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void ControlGrisAudioProcessorEditor::resized() {
    double width = getWidth();
    double height = getHeight();

    double fieldSize = height - 200;
    if (fieldSize < 300) { fieldSize = 300; }

    mainBanner.setBounds(0, 0, fieldSize, 20);
    mainField.setBounds(0, 20, fieldSize, fieldSize);

    if (m_selectedOscFormat == 2) {
        mainBanner.setText("Azimuth - Distance", NotificationType::dontSendNotification);
        elevationBanner.setVisible(true);
        elevationField.setVisible(true);
        elevationBanner.setBounds(fieldSize, 0, fieldSize, 20);
        elevationField.setBounds(fieldSize, 20, fieldSize, fieldSize);
        parametersBanner.setBounds(fieldSize * 2, 0, width - fieldSize * 2, 20);
        parametersBox.setBounds(fieldSize * 2, 20, width-fieldSize * 2, fieldSize);
    } else {
        mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
        elevationBanner.setVisible(false);
        elevationField.setVisible(false);
        parametersBanner.setBounds(fieldSize, 0, width-fieldSize, 20);
        parametersBox.setBounds(fieldSize, 20, width-fieldSize, fieldSize);
    }

    int sash = width > 900 ? width - 450 : 450;

    trajectoryBanner.setBounds(0, fieldSize + 20, sash, 20);
    trajectoryBox.setBounds(0, fieldSize + 40, sash, 160);

    settingsBanner.setBounds(sash, fieldSize + 20, 450, 20);
    configurationComponent.setBounds(sash, fieldSize + 40, 450, 160);

    lastUIWidth  = getWidth();
    lastUIHeight = getHeight();
}
