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

    Random random = Random();
    m_numOfSources = 8;
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setId(i);
        sources[i].setAzimuth(random.nextDouble() * 360.0);
        sources[i].setElevation(random.nextDouble() * 90.0);
        sources[i].setDistance(random.nextDouble());
    }

    mainField.setSources(sources, m_numOfSources);
    elevationField.setSources(sources, m_numOfSources);

    m_selectedSource = 0;
    parametersBox.setSelectedSource(&sources[m_selectedSource]);

    for (int i = 0; i < MaxNumOfSources; i++) {
        valueTreeState.addParameterListener(String("azimuth_") + String(i+1), this);
        valueTreeState.addParameterListener(String("elevation_") + String(i+1), this);
        valueTreeState.addParameterListener(String("distance_") + String(i+1), this);
        valueTreeState.addParameterListener(String("azimuthSpan_") + String(i+1), this);
        valueTreeState.addParameterListener(String("elevationSpan_") + String(i+1), this);
        valueTreeState.addParameterListener(String("x_") + String(i+1), this);
        valueTreeState.addParameterListener(String("y_") + String(i+1), this);
    }

    //setResizable(true, true);
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
    for (int i = 0; i < MaxNumOfSources; i++) {
        sources[i].setRadiusIsElevation(m_selectedOscFormat != 2);
    }
    resized();
}

void ControlGrisAudioProcessorEditor::parameterChanged(int parameterId, double value) {
    switch (parameterId) {
        case 0:
            sources[m_selectedSource].setNormalizedAzimuth(value); break;
        case 1:
            sources[m_selectedSource].setElevation(value * 90.0); break;
        case 2:
            sources[m_selectedSource].setDistance(value); break;
        case 3:
            sources[m_selectedSource].setX(value); break;
        case 4:
            sources[m_selectedSource].setY(value); break;
        case 5:
            sources[m_selectedSource].setAzimuthSpan(value); break;
        case 6:
            sources[m_selectedSource].setElevationSpan(value); break;
    }
    mainField.repaint();
    if (m_selectedOscFormat == 2) {
        elevationField.repaint();
    }
}

void ControlGrisAudioProcessorEditor::parameterChanged(const String &parameterID, float newValue) {
    int sourceId = parameterID.getTrailingIntValue() - 1;
    if (parameterID.startsWith("azimuth_")) {
        sources[sourceId].setNormalizedAzimuth(newValue);
    } else if (parameterID.startsWith("elevation_")) {
        sources[sourceId].setElevation(newValue * 90.0);
    } else if (parameterID.startsWith("distance_")) {
        sources[sourceId].setDistance(newValue);
    } else if (parameterID.startsWith("azimuthSpan_")) {
        sources[sourceId].setAzimuthSpan(newValue);
    } else if (parameterID.startsWith("elevationSpan_")) {
        sources[sourceId].setElevationSpan(newValue);
    } else if (parameterID.startsWith("x_")) {
        sources[sourceId].setX(newValue);
    } else if (parameterID.startsWith("y_")) {
        sources[sourceId].setY(newValue);
    }

    if (sourceId == m_selectedSource) {
        parametersBox.setSelectedSource(&sources[sourceId]);
    }

    mainField.repaint();
    if (m_selectedOscFormat == 2) {
        elevationField.repaint();
    }
}

void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {

    m_selectedSource = sourceId;
    mainField.setSelectedSource(m_selectedSource);
    elevationField.setSelectedSource(m_selectedSource);
    parametersBox.setSelectedSource(&sources[sourceId]);

    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    float azim = -sources[sourceId].getAzimuth() / 360.0 * M_PI * 2.0 + M_PI;
    float elev = (M_PI / 2.0) - (sources[sourceId].getElevation() / 360.0 * M_PI * 2.0);
    
    message.addInt32(sourceId);
    message.addFloat32(azim);
    message.addFloat32(elev);
    message.addFloat32(0.0);
    message.addFloat32(0.0);
    message.addFloat32(sources[sourceId].getDistance());
    message.addFloat32(0.0);

    if (!processor.oscSender.send(message)) {
        std::cout << "Error: could not send OSC message." << std::endl;
        return;
    }
}
