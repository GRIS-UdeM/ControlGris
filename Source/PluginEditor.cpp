/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpatGris2AudioProcessorEditor::SpatGris2AudioProcessorEditor (SpatGris2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (900, 740);

    setLookAndFeel(&mGrisFeel);
 
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

    if (! oscSender.connect("127.0.0.1", 18032)) {
        std::cout << "Error: could not connect to UDP port 18032." << std::endl;
    }

    m_selectedSource = 0;
    parametersBox.setSelectedSource(&sources[m_selectedSource]);

}

SpatGris2AudioProcessorEditor::~SpatGris2AudioProcessorEditor() {
    setLookAndFeel(nullptr);
    oscSender.disconnect();
}

Source * SpatGris2AudioProcessorEditor::getSources() {
    return sources;
}

int SpatGris2AudioProcessorEditor::getSelectedSource() {
    return m_selectedSource;
}

//==============================================================================
void SpatGris2AudioProcessorEditor::paint (Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void SpatGris2AudioProcessorEditor::resized() {
    double width = getWidth();
    double height = getHeight();
 
    double rightComponentWidth = width - 350;

    azimuthElevationBanner.setBounds(0, 0, 350, 20);
    radiusBanner.setBounds(0, 370, 350, 20);

    parametersBanner.setBounds(350, 0, rightComponentWidth, 20);
    trajectoryBanner.setBounds(350, 150, rightComponentWidth, 20);
    settingsBanner.setBounds(350, 300, rightComponentWidth, 20);

    azimuthElevationField.setBounds(0, 20, 350, 350);
    radiusField.setBounds(0, 390, 350, 350);

    parametersBox.setBounds(350, 20, rightComponentWidth, 130);
    trajectoryBox.setBounds(350, 170, rightComponentWidth, 130);

    configurationComponent.setBounds(350, 320, rightComponentWidth, 160);
}

//==============================================================================
void SpatGris2AudioProcessorEditor::parameterChanged(int parameterId, double value) {
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

void SpatGris2AudioProcessorEditor::sourcePositionChanged(int sourceId) {

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

    if (!oscSender.send(message)) {
        std::cout << "Error: could not send OSC message." << std::endl;
        return;
    }
}


