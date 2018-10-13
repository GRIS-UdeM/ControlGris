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

    addAndMakeVisible(&azimuthElevationField);
    addAndMakeVisible(&radiusField);

    addAndMakeVisible(&parametersBox);
}

SpatGris2AudioProcessorEditor::~SpatGris2AudioProcessorEditor()
{
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
}
