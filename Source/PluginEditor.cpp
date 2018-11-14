/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel(&mGrisFeel);
 
    m_selectedOscFormat = 1;

    azimuthElevationBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    radiusBanner.setText("Radius", NotificationType::dontSendNotification);
    parametersBanner.setText("Source Parameters", NotificationType::dontSendNotification);
    trajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    settingsBanner.setText("Configuration", NotificationType::dontSendNotification);

    addAndMakeVisible(&azimuthElevationBanner);
    addAndMakeVisible(&radiusBanner);
    addAndMakeVisible(&parametersBanner);
    addAndMakeVisible(&trajectoryBanner);
    addAndMakeVisible(&settingsBanner);

    azimuthElevationField.addListener(this);
    addAndMakeVisible(&azimuthElevationField);
    addAndMakeVisible(&radiusField);

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
    }

    azimuthElevationField.setSources(sources, m_numOfSources);

    m_selectedSource = 0;
    parametersBox.setSelectedSource(&sources[m_selectedSource]);

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

    azimuthElevationBanner.setBounds(0, 0, 300, 20);
    azimuthElevationField.setBounds(0, 20, 300, 300);

    if (m_selectedOscFormat == 2) {
        radiusBanner.setVisible(true);
        radiusField.setVisible(true);
        radiusBanner.setBounds(300, 0, 300, 20);
        radiusField.setBounds(300, 20, 300, 300);
        parametersBanner.setBounds(600, 0, width-600, 20);
        parametersBox.setBounds(600, 20, width-600, 300);
    } else {
        radiusBanner.setVisible(false);
        radiusField.setVisible(false);
        parametersBanner.setBounds(300, 0, width-300, 20);
        parametersBox.setBounds(300, 20, width-300, 300);
    }

    int sash = width > 900 ? width - 450 : 450;

    trajectoryBanner.setBounds(0, 320, sash, 20);
    trajectoryBox.setBounds(0, 340, sash, 160);

    settingsBanner.setBounds(sash, 320, 450, 20);
    configurationComponent.setBounds(sash, 340, 450, 160);

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
    resized();
}

void ControlGrisAudioProcessorEditor::parameterChanged(int parameterId, double value) {
    switch (parameterId) {
        case 0:
            sources[m_selectedSource].setAzimuth(value * 360.0); break;
        case 1:
            sources[m_selectedSource].setElevation(value * 90.0); break;
        case 2:
            sources[m_selectedSource].setRadius(value); break;
/*
        case 3:
            sources[m_selectedSource].setX(value); break;
        case 4:
            sources[m_selectedSource].setY(value); break;
        case 5:
            sources[m_selectedSource].setZ(value); break;
        case 6:
            sources[m_selectedSource].setAzimuthSpan(value); break;
        case 7:
            sources[m_selectedSource].setElevationSpan(value); break;
*/
    }
    azimuthElevationField.repaint();
}

void ControlGrisAudioProcessorEditor::sourcePositionChanged(int sourceId) {

    m_selectedSource = sourceId;
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
    message.addFloat32(1.0);
    message.addFloat32(0.0);

    if (!processor.oscSender.send(message)) {
        std::cout << "Error: could not send OSC message." << std::endl;
        return;
    }
}


