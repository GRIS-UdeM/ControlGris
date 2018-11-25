/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor& p,
                                                                   AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{
    setLookAndFeel(&mGrisFeel);
 
    m_selectedOscFormat = 1;

    mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    elevationBanner.setText("Elevation", NotificationType::dontSendNotification);
    parametersBanner.setText("Source Parameters", NotificationType::dontSendNotification);
    trajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    settingsBanner.setText("Configuration", NotificationType::dontSendNotification);

    addAndMakeVisible(&mainBanner);
    addAndMakeVisible(&elevationBanner);
    addAndMakeVisible(&parametersBanner);
    addAndMakeVisible(&trajectoryBanner);
    addAndMakeVisible(&settingsBanner);

    mainField.addListener(this);
    addAndMakeVisible(&mainField);
    elevationField.addListener(this);
    addAndMakeVisible(&elevationField);

    parametersBox.addListener(this);
    addAndMakeVisible(&parametersBox);
    addAndMakeVisible(&trajectoryBox);

    settingsBox.addListener(this);
    addAndMakeVisible(configurationComponent);
    Colour bg = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    configurationComponent.addTab ("Settings", bg, &settingsBox, false);
    configurationComponent.addTab ("Source", bg, &sourceBox, false);
    configurationComponent.addTab ("Controllers", bg, &interfaceBox, false);

    m_numOfSources = 2;

    mainField.setSources(sources, m_numOfSources);
    elevationField.setSources(sources, m_numOfSources);

    m_selectedSource = 0;
    parametersBox.setSelectedSource(&sources[m_selectedSource]);

    for (int i = 0; i < m_numOfSources; i++) {
        valueTreeState.addParameterListener(String("azimuth_") + String(i+1), this);
        valueTreeState.addParameterListener(String("elevation_") + String(i+1), this);
        valueTreeState.addParameterListener(String("distance_") + String(i+1), this);
        valueTreeState.addParameterListener(String("azimuthSpan_") + String(i+1), this);
        valueTreeState.addParameterListener(String("elevationSpan_") + String(i+1), this);
        valueTreeState.addParameterListener(String("x_") + String(i+1), this);
        valueTreeState.addParameterListener(String("y_") + String(i+1), this);
        parametersBox.setLinkState(SOURCE_ID_AZIMUTH, valueTreeState.state.getProperty("azimuthLink", false));
        parametersBox.setLinkState(SOURCE_ID_ELEVATION, valueTreeState.state.getProperty("elevationLink", false));
        parametersBox.setLinkState(SOURCE_ID_DISTANCE, valueTreeState.state.getProperty("distanceLink", false));
        parametersBox.setLinkState(SOURCE_ID_X, valueTreeState.state.getProperty("xLink", false));
        parametersBox.setLinkState(SOURCE_ID_Y, valueTreeState.state.getProperty("yLink", false));
        parametersBox.setLinkState(SOURCE_ID_AZIMUTH_SPAN, valueTreeState.state.getProperty("azimuthSpanLink", false));
        parametersBox.setLinkState(SOURCE_ID_ELEVATION_SPAN, valueTreeState.state.getProperty("elevationSpanLink", false));
    }

    setResizeLimits(300, 320, 1800, 1300);

    lastUIWidth .referTo (processor.parameters.state.getChildWithName ("uiState").getPropertyAsValue ("width",  nullptr));
    lastUIHeight.referTo (processor.parameters.state.getChildWithName ("uiState").getPropertyAsValue ("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);

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

bool ControlGrisAudioProcessorEditor::getParameterLinkState(int parameterId) {
    return parametersBox.getLinkState(parameterId);
}

void ControlGrisAudioProcessorEditor::setParameterLinkState(int parameterId, bool state) {
    parametersBox.setLinkState(parameterId, state);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::paint (Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
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
        parametersBanner.setBounds(fieldSize*2, 0, width-fieldSize*2, 20);
        parametersBox.setBounds(fieldSize*2, 20, width-fieldSize*2, fieldSize);
    } else {
        mainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
        elevationBanner.setVisible(false);
        elevationField.setVisible(false);
        parametersBanner.setBounds(fieldSize, 0, width-fieldSize, 20);
        parametersBox.setBounds(fieldSize, 20, width-fieldSize, fieldSize);
    }

    int sash = width > 900 ? width - 450 : 450;

    trajectoryBanner.setBounds(0, fieldSize+20, sash, 20);
    trajectoryBox.setBounds(0, fieldSize+40, sash, 160);

    settingsBanner.setBounds(sash, fieldSize+20, 450, 20);
    configurationComponent.setBounds(sash, fieldSize+40, 450, 160);

    lastUIWidth  = getWidth();
    lastUIHeight = getHeight();
}

//==============================================================================
// called when the stored window size changes
void ControlGrisAudioProcessorEditor::valueChanged (Value&) {
    setSize (lastUIWidth.getValue(), lastUIHeight.getValue());
}

void ControlGrisAudioProcessorEditor::oscFormatChanged(int selectedId) {
    m_selectedOscFormat = selectedId;
    parametersBox.setDistanceEnabled(m_selectedOscFormat == 2);
    mainField.setDrawElevation(m_selectedOscFormat != 2);
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setRadiusIsElevation(m_selectedOscFormat != 2);
    }
    resized();
}

void ControlGrisAudioProcessorEditor::parameterChanged(int parameterId, double value) {
    setSourceParameterValue(m_selectedSource, parameterId, value);
    mainField.repaint();
    if (m_selectedOscFormat == 2) {
        elevationField.repaint();
    }
    sendOscMessage();
}

void ControlGrisAudioProcessorEditor::parameterChanged(const String &parameterID, float newValue) {
    int parameterId;
    int sourceId = parameterID.getTrailingIntValue() - 1;
    if (parameterID.startsWith("azimuth_")) {
        parameterId = SOURCE_ID_AZIMUTH;
    } else if (parameterID.startsWith("elevation_")) {
        parameterId = SOURCE_ID_ELEVATION;
    } else if (parameterID.startsWith("distance_")) {
        parameterId = SOURCE_ID_DISTANCE;
    } else if (parameterID.startsWith("x_")) {
        parameterId = SOURCE_ID_X;
    } else if (parameterID.startsWith("y_")) {
        parameterId = SOURCE_ID_Y;
    } else if (parameterID.startsWith("azimuthSpan_")) {
        parameterId = SOURCE_ID_AZIMUTH_SPAN;
    } else if (parameterID.startsWith("elevationSpan_")) {
        parameterId = SOURCE_ID_ELEVATION_SPAN;
    }
    setSourceParameterValue(sourceId, parameterId, newValue);

    if (sourceId == m_selectedSource) {
        parametersBox.setSelectedSource(&sources[sourceId]);
    }

    mainField.repaint();
    if (m_selectedOscFormat == 2) {
        elevationField.repaint();
    }
    sendOscMessage();
}

void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {
    m_selectedSource = sourceId;
    parametersBox.setSelectedSource(&sources[sourceId]);
    setLinkedParameterValue(sourceId, -1);
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
    sendOscMessage();
}

void ControlGrisAudioProcessorEditor::setSourceParameterValue(int sourceId, int parameterId, double value) {
    switch (parameterId) {
        case SOURCE_ID_AZIMUTH:
            sources[sourceId].setNormalizedAzimuth(value); break;
        case SOURCE_ID_ELEVATION:
            sources[sourceId].setElevation(value * 90.0); break;
        case SOURCE_ID_DISTANCE:
            sources[sourceId].setDistance(value); break;
        case SOURCE_ID_X:
            sources[sourceId].setX(value); break;
        case SOURCE_ID_Y:
            sources[sourceId].setY(value); break;
        case SOURCE_ID_AZIMUTH_SPAN:
            sources[sourceId].setAzimuthSpan(value); break;
        case SOURCE_ID_ELEVATION_SPAN:
            sources[sourceId].setElevationSpan(value); break;
    }
    setLinkedParameterValue(sourceId, parameterId);
}

void ControlGrisAudioProcessorEditor::setLinkedParameterValue(int sourceId, int parameterId) {
    bool linkAzimuth = false, linkElevation = false, linkDistance = false, linkX = false, linkY = false;
    bool linkAzimuthSpan = (parameterId == SOURCE_ID_AZIMUTH_SPAN && parametersBox.getLinkState(SOURCE_ID_AZIMUTH_SPAN));
    bool linkElevationSpan = (parameterId == SOURCE_ID_ELEVATION_SPAN && parametersBox.getLinkState(SOURCE_ID_ELEVATION_SPAN));
    if (parameterId < SOURCE_ID_AZIMUTH) { // Source changed from 2D field view.
        linkAzimuth = parametersBox.getLinkState(SOURCE_ID_AZIMUTH);
        linkElevation = parametersBox.getLinkState(SOURCE_ID_ELEVATION);
        linkDistance = parametersBox.getLinkState(SOURCE_ID_DISTANCE);
        linkX = parametersBox.getLinkState(SOURCE_ID_X);
        linkY = parametersBox.getLinkState(SOURCE_ID_Y);
    } else if (parameterId < SOURCE_ID_X) {
        linkAzimuth = (parameterId == SOURCE_ID_AZIMUTH && parametersBox.getLinkState(SOURCE_ID_AZIMUTH));
        linkElevation = (parameterId == SOURCE_ID_ELEVATION && parametersBox.getLinkState(SOURCE_ID_ELEVATION));
        linkDistance = (parameterId == SOURCE_ID_DISTANCE && parametersBox.getLinkState(SOURCE_ID_DISTANCE));
        linkX = (parametersBox.getLinkState(SOURCE_ID_X));
        linkY = (parametersBox.getLinkState(SOURCE_ID_Y));
    } else if (parameterId < SOURCE_ID_AZIMUTH_SPAN) {
        linkX = (parameterId == SOURCE_ID_X && parametersBox.getLinkState(SOURCE_ID_X));
        linkY = (parameterId == SOURCE_ID_Y && parametersBox.getLinkState(SOURCE_ID_Y));
        linkAzimuth = (parametersBox.getLinkState(SOURCE_ID_AZIMUTH));
        linkElevation = (parametersBox.getLinkState(SOURCE_ID_ELEVATION));
        linkDistance = (parametersBox.getLinkState(SOURCE_ID_DISTANCE));
    }
    for (int i = 0; i < m_numOfSources; i++) {
        if (linkAzimuth)
            sources[i].setAzimuth(sources[sourceId].getAzimuth());
        if (linkElevation)
            sources[i].setElevation(sources[sourceId].getElevation());
        if (linkDistance)
            sources[i].setDistance(sources[sourceId].getDistance());
        if (linkX)
            sources[i].setX(sources[sourceId].getX());
        if (linkY)
            sources[i].setY(sources[sourceId].getY());
        if (linkAzimuthSpan)
            sources[i].setAzimuthSpan(sources[sourceId].getAzimuthSpan());
        if (linkElevationSpan)
            sources[i].setElevationSpan(sources[sourceId].getElevationSpan());
    }
}

void ControlGrisAudioProcessorEditor::sendOscMessage() {
    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    for (int i = 0; i < m_numOfSources; i++) {
        if (sources[i].getChanged()) {
            message.clear();
            float azim = -sources[i].getAzimuth() / 180.0 * M_PI;
            float elev = (M_PI / 2.0) - (sources[i].getElevation() / 360.0 * M_PI * 2.0);
            message.addInt32(i);
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
