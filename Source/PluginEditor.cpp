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
                                                                   AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{ 
    m_selectedSource = 0;

    // Set up the interface.
    //----------------------
    mainBanner.setLookAndFeel(&grisLookAndFeel);
    mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&mainBanner);

    elevationBanner.setLookAndFeel(&grisLookAndFeel);
    elevationBanner.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationBanner);

    parametersBanner.setLookAndFeel(&grisLookAndFeel);
    parametersBanner.setText("Source Parameters", NotificationType::dontSendNotification);
    addAndMakeVisible(&parametersBanner);

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
    addAndMakeVisible(&trajectoryBox);

    settingsBox.setLookAndFeel(&grisLookAndFeel);
    settingsBox.addListener(this);

    sourceBox.setLookAndFeel(&grisLookAndFeel);
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
    startTimerHz(30);
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
    mainField.setDrawElevation(!selectionIsLBAP);
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
    settingsBox.setNumberOfSources(numOfSources);
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);
    mainField.setSources(processor.getSources(), numOfSources);
    elevationField.setSources(processor.getSources(), numOfSources);
}

void ControlGrisAudioProcessorEditor::firstSourceIdChanged(int firstSourceId) {
    processor.setFirstSourceId(firstSourceId);
    settingsBox.setFirstSourceId(firstSourceId);
    parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);

    mainField.repaint();
    if (processor.getOscFormat() == 2)
        elevationField.repaint();
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
}

// Timer callback. Update the interface if anything has changed (mostly automations).
//-----------------------------------------------------------------------------------
void ControlGrisAudioProcessorEditor::timerCallback() {
    if (processor.isSomethingChanged()) {
        parametersBox.setSelectedSource(&processor.getSources()[m_selectedSource]);

        mainField.repaint();
        if (processor.getOscFormat() == 2)
            elevationField.repaint();

        processor.newEventConsumed();
    }
}

// FieldComponent::Listener callback.
//-----------------------------------
void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {
    m_selectedSource = sourceId;
    parametersBox.setSelectedSource(&processor.getSources()[sourceId]);
    //processor.setLinkedParameterValue(sourceId, -1);

    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
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

    if (processor.getOscFormat() == 2) {
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
