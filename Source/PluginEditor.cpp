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

ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor& p,
                                                                  AudioProcessorValueTreeState& vts,
                                                                  AutomationManager& automan)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts), automationManager (automan), mainField (automan)
{ 
    setLookAndFeel(&grisLookAndFeel);

    m_selectedSource = 0;
    m_lastTime = 0.0;

    // Set up the interface.
    //----------------------
    mainBanner.setLookAndFeel(&grisLookAndFeel);
    mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&mainBanner);

    elevationBanner.setLookAndFeel(&grisLookAndFeel);
    elevationBanner.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationBanner);

    trajectoryBanner.setLookAndFeel(&grisLookAndFeel);
    trajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryBanner);

    settingsBanner.setLookAndFeel(&grisLookAndFeel);
    settingsBanner.setText("Configuration", NotificationType::dontSendNotification);
    addAndMakeVisible(&settingsBanner);

    mainField.setLookAndFeel(&grisLookAndFeel);
    mainField.addListener(this);
    addAndMakeVisible(&mainField);

    elevationField.setLookAndFeel(&grisLookAndFeel);
    elevationField.addListener(this);
    addAndMakeVisible(&elevationField);

    parametersBox.setLookAndFeel(&grisLookAndFeel);
    parametersBox.addListener(this);
    addAndMakeVisible(&parametersBox);

    trajectoryBox.setLookAndFeel(&grisLookAndFeel);
    trajectoryBox.addListener(this);
    addAndMakeVisible(trajectoryBox);

    settingsBox.setLookAndFeel(&grisLookAndFeel);
    settingsBox.addListener(this);

    sourceBox.setLookAndFeel(&grisLookAndFeel);
    sourceBox.addListener(this);

    interfaceBox.setLookAndFeel(&grisLookAndFeel);

    Colour bg = grisLookAndFeel.findColour (ResizableWindow::backgroundColourId);

    configurationComponent.setLookAndFeel(&grisLookAndFeel);
    configurationComponent.setColour(TabbedComponent::backgroundColourId, bg);
    configurationComponent.addTab ("Settings", bg, &settingsBox, false);
    configurationComponent.addTab ("Source", bg, &sourceBox, false);
    configurationComponent.addTab ("Controllers", bg, &interfaceBox, false);
    addAndMakeVisible(configurationComponent);

    // Add sources to the fields.
    //---------------------------
    mainField.setSources(processor.getSources(), processor.getNumberOfSources());
    elevationField.setSources(processor.getSources(), processor.getNumberOfSources());

    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);
    processor.setSelectedSourceId(m_selectedSource);

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

    // The timer's callback update the interface if anything has changed.
    //-------------------------------------------------------------------
    startTimerHz(50);
}

ControlGrisAudioProcessorEditor::~ControlGrisAudioProcessorEditor() {}

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
    oscFormatChanged(processor.getOscFormat());
    oscPortNumberChanged(processor.getOscPortNumber());
    oscActivated(processor.getOscConnected());
    numberOfSourcesChanged(processor.getNumberOfSources());
    firstSourceIdChanged(processor.getFirstSourceId());

    // Update the interface.
    //----------------------
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
    processor.setSelectedSourceId(m_selectedSource);
}

// Value::Listener callback. Called when the stored window size changes.
//----------------------------------------------------------------------
void ControlGrisAudioProcessorEditor::valueChanged (Value&) {
    setSize (lastUIWidth.getValue(), lastUIHeight.getValue());
}

// SettingsBoxComponent::Listener callbacks.
//------------------------------------------
void ControlGrisAudioProcessorEditor::oscFormatChanged(int selectedId) {
    processor.setOscFormat(selectedId);
    settingsBox.setOscFormat(selectedId);
    bool selectionIsLBAP = selectedId == 2;
    parametersBox.setDistanceEnabled(selectionIsLBAP);
    mainField.setSpatMode((SpatModeEnum)(selectedId - 1));
    repaint();
    resized();
}

void ControlGrisAudioProcessorEditor::oscPortNumberChanged(int oscPort) {
    processor.setOscPortNumber(oscPort);
    settingsBox.setOscPortNumber(oscPort);
}

void ControlGrisAudioProcessorEditor::oscActivated(bool state) {
    processor.handleOscConnection(state);
    settingsBox.setActivateButtonState(processor.getOscConnected());
}

void ControlGrisAudioProcessorEditor::numberOfSourcesChanged(int numOfSources) {
    m_selectedSource = 0;
    processor.setNumberOfSources(numOfSources);
    processor.setSelectedSourceId(m_selectedSource);
    settingsBox.setNumberOfSources(numOfSources);
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);
    mainField.setSources(processor.getSources(), numOfSources);
    elevationField.setSources(processor.getSources(), numOfSources);
    sourceBox.setNumberOfSources(numOfSources);
}

void ControlGrisAudioProcessorEditor::firstSourceIdChanged(int firstSourceId) {
    processor.setFirstSourceId(firstSourceId);
    settingsBox.setFirstSourceId(firstSourceId);
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);

    mainField.repaint();
    if (processor.getOscFormat() == 2)
        elevationField.repaint();
}

// SourceBoxComponent::Listener callbacks.
//----------------------------------------
void ControlGrisAudioProcessorEditor::sourcePlacementChanged(int value) {
    float azimuth;
    int numOfSources = processor.getNumberOfSources();
    const float azims2[2] = {-90.0f, 90.0f};
    const float azims4[4] = {-45.0f, 45.0f, -135.0f, 135.0f};
    const float azims6[6] = {-30.0f, 30.0f, -90.0f, 90.0f, -150.0f, 150.0f};
    const float azims8[8] = {-22.5f, 22.5f, -67.5f, 67.5f, -112.5f, 112.5f, -157.5f, 157.5f};

    float offset = 360.0f / numOfSources / 2.0f;

    switch(value) {
        case 1:
            for (int i = 0; i < numOfSources; i++) {
                if (numOfSources <= 2)
                    processor.getSources()[i].setCoordinates(-azims2[i], 0.0f, 1.0f);
                else if (numOfSources <= 4)
                    processor.getSources()[i].setCoordinates(-azims4[i], 0.0f, 1.0f);
                else if (numOfSources <= 6)
                    processor.getSources()[i].setCoordinates(-azims6[i], 0.0f, 1.0f);
                else
                    processor.getSources()[i].setCoordinates(-azims8[i], 0.0f, 1.0f);
            }
            break;
        case 2:
            for (int i = 0; i < numOfSources; i++) {
                if (numOfSources <= 2)
                    processor.getSources()[i].setCoordinates(azims2[i], 0.0f, 1.0f);
                else if (numOfSources <= 4)
                    processor.getSources()[i].setCoordinates(azims4[i], 0.0f, 1.0f);
                else if (numOfSources <= 6)
                    processor.getSources()[i].setCoordinates(azims6[i], 0.0f, 1.0f);
                else
                    processor.getSources()[i].setCoordinates(azims8[i], 0.0f, 1.0f);
            }
            break;
        case 3:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * -i + offset, 0.0f, 1.0f);
            }
            break;
        case 4:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * i + offset, 0.0f, 1.0f);
            }
            break;
        case 5:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * -i - offset, 0.0f, 1.0f);
            }
            break;
        case 6:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * i - offset, 0.0f, 1.0f);
            }
            break;
        case 7:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * -i, 0.0f, 1.0f);
            }
            break;
        case 8:
            for (int i = 0; i < numOfSources; i++) {
                processor.getSources()[i].setCoordinates(360.0f / numOfSources * i, 0.0f, 1.0f);
            }
            break;
    }
    repaint();
}

void ControlGrisAudioProcessorEditor::sourceNumberPositionChanged(int sourceNum, float angle, float rayLen) {
    if (processor.getOscFormat() == 2) {
        processor.getSources()[sourceNum-1].setCoordinates(angle, 0.0f, rayLen * 1.4f);
    } else {
        processor.getSources()[sourceNum-1].setCoordinates(angle, 90.0f - (rayLen * 90.0f), 1.0f);
    }
    repaint();
}

// ParametersBoxComponent::Listener callbacks.
//--------------------------------------------
void ControlGrisAudioProcessorEditor::parameterLinkChanged(int parameterId, bool value) {
    StringArray parameterNames({"azimuthLink", "elevationLink", "distanceLink", "xLink",
                                "yLink", "azimuthSpanLink", "elevationSpanLink"});
    valueTreeState.state.setProperty(parameterNames[parameterId], value, nullptr);
}

void ControlGrisAudioProcessorEditor::parameterChanged(int parameterId, double value) {
    processor.setSourceParameterValue(m_selectedSource, parameterId, value);

    mainField.repaint();
    if (processor.getOscFormat() == 2)
        elevationField.repaint();
}

void ControlGrisAudioProcessorEditor::selectedSourceClicked() {
    m_selectedSource = (m_selectedSource + 1) % processor.getNumberOfSources();
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
    processor.setSelectedSourceId(m_selectedSource);
}

// TrajectoryBoxComponent::Listener callbacks.
//--------------------------------------------
void ControlGrisAudioProcessorEditor::trajectoryBoxSourceLinkChanged(int value) {
    int numOfSources = processor.getNumberOfSources();

    if (value == 3) {
        if (processor.getOscFormat() == 2) {
            for (int i = 1; i < numOfSources; i++) {
                processor.getSources()[i].setDistance(processor.getSources()[0].getDistance());
            }
        } else {
            for (int i = 1; i < numOfSources; i++) {
                processor.getSources()[i].setElevation(processor.getSources()[0].getElevation());
            }
        }
    }

    for (int i = 0; i < numOfSources; i++) {
        processor.getSources()[i].fixSourcePosition(value);
    }
    automationManager.setSourceLink(value);
    mainField.repaint();
}

void ControlGrisAudioProcessorEditor::trajectoryBoxTrajectoryTypeChanged(int value) {
    automationManager.setDrawingType(value);
    mainField.repaint();
}

void ControlGrisAudioProcessorEditor::trajectoryBoxDurationChanged(double value) {
    automationManager.setPlaybackDuration(value);
}

void ControlGrisAudioProcessorEditor::trajectoryBoxNumOfCycleChanged(int value) {
    automationManager.setNumberOfCycles(value);
}

void ControlGrisAudioProcessorEditor::trajectoryBoxActivateChanged(bool value) {
    automationManager.setActivateState(value);
}

void ControlGrisAudioProcessorEditor::trajectoryBoxClearButtonClicked() {
    automationManager.resetRecordingTrajectory(Point<float> (150 - kSourceRadius, 150 - kSourceRadius));
    mainField.repaint();
}

// Timer callback. Update the interface if anything has changed (mostly automations).
//-----------------------------------------------------------------------------------
void ControlGrisAudioProcessorEditor::timerCallback() {
    bool needRepaint = false;
    if (processor.isSomethingChanged()) {
        parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);

        needRepaint = true;
        if (processor.getOscFormat() == 2)
            elevationField.repaint();

        processor.newEventConsumed();
    }

    mainField.setIsPlaying(processor.getIsPlaying());

    if (automationManager.getActivateState()) {
        if (m_lastTime != processor.getCurrentTime()) {
            automationManager.setTrajectoryDeltaTime(processor.getCurrentTime() - processor.getInitTimeOnPlay());
            needRepaint = true;
        }
        m_lastTime = processor.getCurrentTime();
    } else if (automationManager.hasValidPlaybackPosition()) {
        automationManager.setSourcePosition(automationManager.getPlaybackPosition());
        needRepaint = true;
    }

    if (needRepaint)
        mainField.repaint();
}

// FieldComponent::Listener callback.
//-----------------------------------
void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {
    m_selectedSource = sourceId;
    parametersBox.setSelectedSource(&processor.getSources()[sourceId]);
    processor.setSourceParameterValue(sourceId, SOURCE_ID_AZIMUTH, processor.getSources()[sourceId].getNormalizedAzimuth());
    processor.setSourceParameterValue(sourceId, SOURCE_ID_ELEVATION, processor.getSources()[sourceId].getNormalizedElevation());
    processor.setSourceParameterValue(sourceId, SOURCE_ID_DISTANCE, processor.getSources()[sourceId].getDistance());

    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
    processor.setSelectedSourceId(m_selectedSource);

    // Test fixed radius...
    if (automationManager.getSourceLink() == 3) {
        int numOfSources = processor.getNumberOfSources();
        if (processor.getOscFormat() == 2) {
            for (int i = 1; i < numOfSources; i++) {
                processor.getSources()[i].setDistance(processor.getSources()[0].getDistance());
            }
        } else {
            for (int i = 1; i < numOfSources; i++) {
                processor.getSources()[i].setElevation(processor.getSources()[0].getElevation());
            }
        }
        for (int i = 0; i < numOfSources; i++) {
            processor.getSources()[i].fixSourcePosition(automationManager.getSourceLink());
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::paint (Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void ControlGrisAudioProcessorEditor::resized() {
    double width = getWidth();
    double height = getHeight();

    double fieldSize = width / 2;
    if (fieldSize < 300) { fieldSize = 300; }

    mainBanner.setBounds(0, 0, fieldSize, 20);
    mainField.setBounds(0, 20, fieldSize, fieldSize);

    if (processor.getOscFormat() == 2) {
        mainBanner.setText("Azimuth - Distance", NotificationType::dontSendNotification);
        elevationBanner.setVisible(true);
        elevationField.setVisible(true);
        elevationBanner.setBounds(fieldSize, 0, fieldSize, 20);
        elevationField.setBounds(fieldSize, 20, fieldSize, fieldSize);
    } else {
        mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
        elevationBanner.setVisible(false);
        elevationField.setVisible(false);
    }

    parametersBox.setBounds(0, fieldSize + 20, width, 50);

    trajectoryBanner.setBounds(0, fieldSize + 70, width, 20);
    trajectoryBox.setBounds(0, fieldSize + 90, width, 140);

    settingsBanner.setBounds(0, fieldSize + 230, width, 20);
    configurationComponent.setBounds(0, fieldSize + 250, width, 130);

    lastUIWidth  = getWidth();
    lastUIHeight = getHeight();
}
