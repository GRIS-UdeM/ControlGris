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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (900, 740);

    setLookAndFeel(&mGrisFeel);

    azimuthElevationBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    radiusBanner.setText("Radius", NotificationType::dontSendNotification);
    parametersBanner.setText("Source Parameters", NotificationType::dontSendNotification);
    trajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    settingsBanner.setText("Settings", NotificationType::dontSendNotification);
    sourceBanner.setText("Sources", NotificationType::dontSendNotification);
    interfaceBanner.setText("Interfaces", NotificationType::dontSendNotification);

    addAndMakeVisible(&azimuthElevationBanner);
    addAndMakeVisible(&radiusBanner);
    addAndMakeVisible(&parametersBanner);
    addAndMakeVisible(&trajectoryBanner);
    addAndMakeVisible(&settingsBanner);
    addAndMakeVisible(&sourceBanner);
    addAndMakeVisible(&interfaceBanner);

    azimuthElevationField.addListener(this);
    addAndMakeVisible(&azimuthElevationField);
    addAndMakeVisible(&radiusField);

    addAndMakeVisible(&parametersBox);
    addAndMakeVisible(&trajectoryBox);
    addAndMakeVisible(&settingsBox);
    addAndMakeVisible(&sourceBox);
    addAndMakeVisible(&interfaceBox);

    Random random = Random();
    m_numOfSources = 8;
    for (int i = 0; i < m_numOfSources; i++) {
        sources[i].setAzimuth(random.nextDouble() * 360.0);
        sources[i].setElevation(random.nextDouble() * 90.0);
    }

    azimuthElevationField.setSources(sources, m_numOfSources);

    if (! oscSender.connect("127.0.0.1", 18032)) {
        std::cout << "Error: could not connect to UDP port 18032." << std::endl;
    }

}

SpatGris2AudioProcessorEditor::~SpatGris2AudioProcessorEditor() {
    setLookAndFeel(nullptr);
    oscSender.disconnect();
}

Source * SpatGris2AudioProcessorEditor::getSources() {
    return sources;
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
    sourceBanner.setBounds(350, 450, rightComponentWidth, 20);
    interfaceBanner.setBounds(350, 600, rightComponentWidth, 20);

    azimuthElevationField.setBounds(0, 20, 350, 350);
    radiusField.setBounds(0, 390, 350, 350);

    parametersBox.setBounds(350, 20, rightComponentWidth, 130);
    trajectoryBox.setBounds(350, 170, rightComponentWidth, 130);
    settingsBox.setBounds(350, 320, rightComponentWidth, 130);
    sourceBox.setBounds(350, 470, rightComponentWidth, 130);
    interfaceBox.setBounds(350, 620, rightComponentWidth, 130);

}

//==============================================================================
void SpatGris2AudioProcessorEditor::sourcePositionChanged(int sourceId) {
    std::cout << sourceId << " " << sources[sourceId].getAzimuth() << " " << sources[sourceId].getElevation() << std::endl;

    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    float azim = -sources[sourceId].getAzimuth() / 360.0 * M_PI * 2.0 + M_PI;
    float elev = (M_PI / 2.0) - (sources[sourceId].getElevation() / 360.0 * M_PI * 2.0);
    
    std::cout << azim << std::endl;
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


