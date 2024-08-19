/*
 This file is part of ControlGris.

 Developers: Gaël LANE LÉPINE

 ControlGris is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.

 ControlGris is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with ControlGris.  If not, see
 <http://www.gnu.org/licenses/>.
*/

//==============================================================================

#include "cg_SectionSoundReactiveSpatialization.h"

//==============================================================================
gris::SectionSoundReactiveSpatialization::SectionSoundReactiveSpatialization(GrisLookAndFeel & grisLookAndFeel,
    ControlGrisAudioProcessor & audioProcessor)
    : mGrisLookAndFeel(grisLookAndFeel)
    , mAudioProcessor(audioProcessor)
    , mDescriptorFactorSlider(grisLookAndFeel)
    , mDescriptorThresholdSlider(grisLookAndFeel)
    , mDescriptorMinFreqSlider(grisLookAndFeel)
    , mDescriptorMaxFreqSlider(grisLookAndFeel)
    , mDescriptorMinTimeSlider(grisLookAndFeel)
    , mDescriptorMaxTimeSlider(grisLookAndFeel)
    , mDescriptorSmoothSlider(grisLookAndFeel)
    , mDescriptorSmoothCoefSlider(grisLookAndFeel)
    , mParameterAzimuthRangeSlider(grisLookAndFeel)
    , mParameterElevationRangeSlider(grisLookAndFeel)
    , mParameterXRangeSlider(grisLookAndFeel)
    , mParameterYRangeSlider(grisLookAndFeel)
    , mParameterZRangeSlider(grisLookAndFeel)
    , mParameterAzimuthOrXYSpanRangeSlider(grisLookAndFeel)
    , mParameterElevationOrZSpanRangeSlider(grisLookAndFeel)
    , mParameterElevationZOffsetSlider(grisLookAndFeel)
    , mParameterEleZSpanOffsetSlider(grisLookAndFeel)
    //, mParameterLapSlider(grisLookAndFeel)
{
    auto const initRangeSlider = [&](NumSlider & slider) {
        slider.setNormalisableRange(juce::NormalisableRange<double>{ -100.0, 100.0, 0.1 });
        slider.setValue(100.0, juce::dontSendNotification);
        slider.setNumDecimalPlacesToDisplay(1);
        addAndMakeVisible(slider);
    };

    auto const initParameterDescCombo = [&](juce::ComboBox & combo) {
        //combo.setTextWhenNothingSelected("-");
        combo.addItemList(AUDIO_DESCRIPTOR_TYPES, 1);
        combo.setSelectedId(1, juce::dontSendNotification);
        addAndMakeVisible(combo);
    };

    setName("Sound Reactive Spatialization");

    addAndMakeVisible(&mSpatialParameterLabel);
    mSpatialParameterLabel.setText("Spatial Parameters", juce::dontSendNotification);
    addAndMakeVisible(&mAudioAnalysisLabel);
    mAudioAnalysisLabel.setText("Audio Analysis", juce::dontSendNotification);

    //==============================================================================
    // Spatial Parameters

    // default values
    mParameterAzimuthRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterElevationRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterXRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterYRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterZRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterAzimuthOrXYSpanRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterElevationOrZSpanRangeSlider.setDoubleClickReturnValue(true, 100.0);
    mParameterElevationZOffsetSlider.setDoubleClickReturnValue(true, 0.0);
    mParameterEleZSpanOffsetSlider.setDoubleClickReturnValue(true, 0.0);

    mParameterAzimuthRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterElevationRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterXRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterYRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterZRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterAzimuthOrXYSpanRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterElevationOrZSpanRangeSlider.setNumDecimalPlacesToDisplay(3);
    mParameterElevationZOffsetSlider.setNumDecimalPlacesToDisplay(3);
    mParameterEleZSpanOffsetSlider.setNumDecimalPlacesToDisplay(3);

    addAndMakeVisible(&mParameterAzimuthButton);
    mParameterAzimuthButton.setButtonText("Azimuth");
    //mParameterAzimuthButton.setClickingTogglesState(true);
    //mParameterAzimuthButton.onClick = 

    addAndMakeVisible(&mParameterElevationButton);
    mParameterElevationButton.setButtonText("Elevation");
    //mParameterElevationButton.setClickingTogglesState(true);

    addAndMakeVisible(&mParameterXButton);
    mParameterXButton.setButtonText("X");
    //mParameterXButton.setClickingTogglesState(true);

    addAndMakeVisible(&mParameterYButton);
    mParameterYButton.setButtonText("Y");
    //mParameterYButton.setClickingTogglesState(true);
    
    addAndMakeVisible(&mParameterZButton);
    mParameterZButton.setButtonText("Z");
    //mParameterZButton.setClickingTogglesState(true);

    addAndMakeVisible(&mParameterAzimuthOrXYSpanButton);
    mParameterAzimuthOrXYSpanButton.setButtonText("Azimuth Span");
    //mParameterAzimuthOrXYSpanButton.setClickingTogglesState(true);

    addAndMakeVisible(&mParameterElevationOrZSpanButton);
    mParameterElevationOrZSpanButton.setButtonText("Elevation Span");
    //mParameterElevationOrZSpanButton.setClickingTogglesState(true);

    initParameterDescCombo(mParameterAzimuthDescriptorCombo);
    mParameterAzimuthDescriptorCombo.onChange = [this] {
        mParameterToShow = mAudioProcessor.getAzimuthDome();
        refreshDescriptorPanel();
    };

    initParameterDescCombo(mParameterElevationDescriptorCombo);
    mParameterElevationDescriptorCombo.onChange = [this] {
        mParameterToShow = mAudioProcessor.getElevationDome();
        refreshDescriptorPanel();
    };

    initParameterDescCombo(mParameterXDescriptorCombo);
    mParameterXDescriptorCombo.onChange = [this] {
        mParameterToShow = mAudioProcessor.getXCube();
        refreshDescriptorPanel();
    };
    
    initParameterDescCombo(mParameterYDescriptorCombo);
    mParameterYDescriptorCombo.onChange = [this] {
        mParameterToShow = mAudioProcessor.getYCube();
        refreshDescriptorPanel();
    };

    initParameterDescCombo(mParameterZDescriptorCombo);
    mParameterZDescriptorCombo.onChange = [this] {
        mParameterToShow = mAudioProcessor.getZCube();
        refreshDescriptorPanel();
    };

    initParameterDescCombo(mParameterAzimuthOrXYSpanDescriptorCombo);
    mParameterAzimuthOrXYSpanDescriptorCombo.onChange = [this] {
        if (mSpatMode == SpatMode::dome) {
            mParameterToShow = mAudioProcessor.getHSpanDome();
        } else {
            mParameterToShow = mAudioProcessor.getHSpanCube();
        }
        refreshDescriptorPanel();
    };

    initParameterDescCombo(mParameterElevationOrZSpanDescriptorCombo);
    mParameterElevationOrZSpanDescriptorCombo.onChange = [this] {
        if (mSpatMode == SpatMode::dome) {
            mParameterToShow = mAudioProcessor.getVSpanDome();
        } else {
            mParameterToShow = mAudioProcessor.getVSpanCube();
        }
        refreshDescriptorPanel();
    };

    //mSourcePlacementCombo.onChange = [this] {
    //    mListeners.call([&](Listener & l) {
    //        l.sourcesPlacementChangedCallback(static_cast<SourcePlacement>(mSourcePlacementCombo.getSelectedId()));
    //        mSourcePlacementCombo.setSelectedId(0, juce::NotificationType::dontSendNotification);
    //    });
    //};

    addAndMakeVisible(&mParameterRangeLabel);
    mParameterRangeLabel.setText("Range", juce::dontSendNotification);

    initRangeSlider(mParameterAzimuthRangeSlider);
    initRangeSlider(mParameterElevationRangeSlider);
    initRangeSlider(mParameterXRangeSlider);
    initRangeSlider(mParameterYRangeSlider);
    initRangeSlider(mParameterZRangeSlider);
    initRangeSlider(mParameterAzimuthOrXYSpanRangeSlider);
    initRangeSlider(mParameterElevationOrZSpanRangeSlider);

    addAndMakeVisible(&mParameterOffsetLabel);
    mParameterOffsetLabel.setText("Offset", juce::dontSendNotification);

    addAndMakeVisible(&mParameterElevationZOffsetSlider);
    mParameterElevationZOffsetSlider.setNormalisableRange(juce::NormalisableRange<double>{ 0.0, 90.0, 0.1 });
    mParameterElevationZOffsetSlider.setValue(0.0, juce::dontSendNotification);

    addAndMakeVisible(&mParameterEleZSpanOffsetSlider);
    mParameterEleZSpanOffsetSlider.setNormalisableRange(juce::NormalisableRange<double>{ 0.0, 1.0, 0.01 });
    mParameterEleZSpanOffsetSlider.setValue(0.0, juce::dontSendNotification);

    addAndMakeVisible(&mParameterLapLabel);
    mParameterLapLabel.setText("Lap", juce::dontSendNotification);

    addAndMakeVisible(&mParameterLapCombo);
    mParameterLapCombo.addItemList({ "1", "2", "3", "4" }, 1);
    mParameterLapCombo.setSelectedId(1);

    //==============================================================================
    // Audio Analysis

    // default values
    mDescriptorFactorSlider.setDoubleClickReturnValue(true, 100.0);
    mDescriptorSmoothSlider.setDoubleClickReturnValue(true, 5.0);
    mDescriptorSmoothCoefSlider.setDoubleClickReturnValue(true, 0.0);
    mDescriptorMinFreqSlider.setDoubleClickReturnValue(true, 20.0);
    mDescriptorMaxFreqSlider.setDoubleClickReturnValue(true, 10000.0);
    mDescriptorThresholdSlider.setDoubleClickReturnValue(true, 0.1);
    mDescriptorMinTimeSlider.setDoubleClickReturnValue(true, 0.1);
    mDescriptorMaxTimeSlider.setDoubleClickReturnValue(true, 10.0);

    mDescriptorFactorSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorSmoothSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorSmoothCoefSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorMinFreqSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorMaxFreqSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorThresholdSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorMinTimeSlider.setNumDecimalPlacesToDisplay(3);
    mDescriptorMaxTimeSlider.setNumDecimalPlacesToDisplay(3);

    addAndMakeVisible(&mDescriptorFactorSlider);
    addAndMakeVisible(&mDescriptorSmoothSlider);
    addAndMakeVisible(&mDescriptorSmoothCoefSlider);
    addAndMakeVisible(&mDescriptorMinFreqSlider);
    addAndMakeVisible(&mDescriptorMaxFreqSlider);
    addAndMakeVisible(&mDescriptorThresholdSlider);
    addAndMakeVisible(&mDescriptorMinTimeSlider);
    addAndMakeVisible(&mDescriptorMaxTimeSlider);

    setSpatMode(mAudioProcessor.getSpatMode());
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));

    //g.setColour(mGrisLookAndFeel.getDarkColor());
    g.setColour(juce::Colour(100, 100, 100));
    //g.drawLine(juce::Line<float>(0.0f, 20.0f, (float)getWidth(), 20.0f));
    g.drawHorizontalLine(20, 0.0f, static_cast<float>(getWidth()));
    g.drawVerticalLine(350, 0.0f, static_cast<float>(getHeight()));

    // linked parameters
    if (mSpatMode == SpatMode::cube) {
        if (mXYParamLinked) {
            g.setColour(juce::Colours::orange);
            g.drawLine(juce::Line<float>(5.0f, 42.0f, 17.0f, 42.0f), 2.0f);
            g.drawLine(juce::Line<float>(5.0f, 62.0f, 17.0f, 62.0f), 2.0f);
            g.drawLine(juce::Line<float>(5.0f, 42.0f, 5.0f, 62.0f), 2.0f);
        } else {
            g.setColour(juce::Colours::black);
            float dashLengths[4] = { 2.0f, 2.0f, 2.0f, 2.0f };
            g.drawDashedLine(juce::Line<float>(5.0f, 42.0f, 17.0f, 42.0f), dashLengths, 4, 2.0f);
            g.drawDashedLine(juce::Line<float>(5.0f, 62.0f, 17.0f, 62.0f), dashLengths, 4, 2.0f);
            g.drawDashedLine(juce::Line<float>(6.0f, 41.0f, 6.0f, 61.0f), dashLengths, 4, 2.0f);
        }
    }
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::resized()
{
    auto area{ getLocalBounds() };
    auto bannerArea{ area.removeFromTop(20) };
    auto bannerSpatParam{ bannerArea.removeFromLeft(350) };
    auto bannerAudioAnalysis{ bannerArea };
    auto areaSpatParams{ area.removeFromLeft(350) };
    auto areaAudioAnalysis{ area };

    mSpatialParameterLabel.setBounds(5, 3, 140, 15);
    mAudioAnalysisLabel.setBounds(bannerAudioAnalysis.getTopLeft().getX() + 5, 3, 140, 15);

    mParameterYButton.setEnabled(true);
    mParameterYDescriptorCombo.setEnabled(true);
    mParameterYRangeSlider.setEnabled(true);

    if (mSpatMode == SpatMode::dome) {
        mParameterXButton.setVisible(false);
        mParameterYButton.setVisible(false);
        mParameterZButton.setVisible(false);
        mParameterAzimuthButton.setVisible(true);
        mParameterElevationButton.setVisible(true);
        mParameterAzimuthDescriptorCombo.setVisible(true);
        mParameterElevationDescriptorCombo.setVisible(true);
        mParameterXDescriptorCombo.setVisible(false);
        mParameterYDescriptorCombo.setVisible(false);
        mParameterZDescriptorCombo.setVisible(false);
        mParameterAzimuthRangeSlider.setVisible(true);
        mParameterElevationRangeSlider.setVisible(true);
        mParameterXRangeSlider.setVisible(false);
        mParameterYRangeSlider.setVisible(false);
        mParameterZRangeSlider.setVisible(false);
        mParameterLapLabel.setVisible(true);
        mParameterLapCombo.setVisible(true);

        mParameterAzimuthButton.setBounds(areaSpatParams.getTopLeft().getX() + 17,
                                          areaSpatParams.getTopLeft().getY() + 15,
                                          80,
                                          15);
        mParameterElevationButton.setBounds(mParameterAzimuthButton.getBounds().getTopLeft().getX(),
                                            mParameterAzimuthButton.getBounds().getBottom() + 5,
                                            80,
                                            15);
        mParameterAzimuthOrXYSpanButton.setBounds(mParameterAzimuthButton.getBounds().getTopLeft().getX(),
                                                  mParameterElevationButton.getBounds().getBottom() + 5,
                                                  80,
                                                  15);
        mParameterElevationOrZSpanButton.setBounds(mParameterAzimuthButton.getBounds().getTopLeft().getX(),
                                                   mParameterAzimuthOrXYSpanButton.getBounds().getBottom() + 5,
                                                   80,
                                                   15);

        mParameterAzimuthDescriptorCombo.setBounds(mParameterAzimuthButton.getRight() + 5,
                                         areaSpatParams.getTopLeft().getY() + 15,
                                         100,
                                         15);
        mParameterElevationDescriptorCombo.setBounds(mParameterAzimuthButton.getRight() + 5,
                                           mParameterAzimuthDescriptorCombo.getBounds().getBottom() + 5,
                                           100,
                                           15);
        mParameterAzimuthOrXYSpanDescriptorCombo.setBounds(mParameterAzimuthButton.getRight() + 5,
                                                 mParameterElevationDescriptorCombo.getBounds().getBottom() + 5,
                                                 100,
                                                 15);
        mParameterElevationOrZSpanDescriptorCombo.setBounds(mParameterAzimuthButton.getRight() + 5,
                                                  mParameterAzimuthOrXYSpanDescriptorCombo.getBounds().getBottom() + 5,
                                                  100,
                                                  15);

        mParameterRangeLabel.setBounds(mParameterAzimuthDescriptorCombo.getRight() + 3,
                                              areaSpatParams.getTopLeft().getY() + 2,
                                              40,
                                              15);

        mParameterAzimuthRangeSlider.setBounds(mParameterAzimuthDescriptorCombo.getRight() + 5,
                                               areaSpatParams.getTopLeft().getY() + 17,
                                               35,
                                               12);
        mParameterElevationRangeSlider.setBounds(mParameterAzimuthDescriptorCombo.getRight() + 5,
                                                 mParameterAzimuthRangeSlider.getBounds().getBottomLeft().getY() + 8,
                                                 35,
                                                 12);
        mParameterAzimuthOrXYSpanRangeSlider.setBounds(mParameterAzimuthDescriptorCombo.getRight() + 5,
                                                       mParameterElevationRangeSlider.getBounds().getBottomLeft().getY()
                                                           + 8,
                                                       35,
                                                       12);
        mParameterElevationOrZSpanRangeSlider.setBounds(
            mParameterAzimuthDescriptorCombo.getRight() + 5,
            mParameterAzimuthOrXYSpanRangeSlider.getBounds().getBottomLeft().getY() + 8,
            35,
            12);

        mParameterOffsetLabel.setBounds(mParameterAzimuthRangeSlider.getRight() + 3,
                                        areaSpatParams.getTopLeft().getY() + 2,
                                        40,
                                        15);

        mParameterElevationZOffsetSlider.setBounds(mParameterAzimuthRangeSlider.getRight() + 5,
                                                   mParameterElevationRangeSlider.getBounds().getTopLeft().getY(),
                                                   35,
                                                   12);
        mParameterEleZSpanOffsetSlider.setBounds(mParameterAzimuthRangeSlider.getRight() + 5,
                                                 mParameterElevationOrZSpanRangeSlider.getBounds().getTopLeft().getY(),
                                                 35,
                                                 12);

        mParameterLapLabel.setBounds(mParameterOffsetLabel.getRight() + 7,
                                     areaSpatParams.getTopLeft().getY() + 2,
                                     40,
                                     15);

        mParameterLapCombo.setBounds(mParameterOffsetLabel.getRight() + 5,
                                     mParameterAzimuthDescriptorCombo.getBounds().getTopLeft().getY(),
                                     30,
                                     15);
    } else {
        mParameterXButton.setVisible(true);
        mParameterYButton.setVisible(true);
        mParameterZButton.setVisible(true);
        mParameterAzimuthButton.setVisible(false);
        mParameterElevationButton.setVisible(false);
        mParameterAzimuthDescriptorCombo.setVisible(false);
        mParameterElevationDescriptorCombo.setVisible(false);
        mParameterXDescriptorCombo.setVisible(true);
        mParameterYDescriptorCombo.setVisible(true);
        mParameterZDescriptorCombo.setVisible(true);
        mParameterAzimuthRangeSlider.setVisible(false);
        mParameterElevationRangeSlider.setVisible(false);
        mParameterXRangeSlider.setVisible(true);
        mParameterYRangeSlider.setVisible(true);
        mParameterZRangeSlider.setVisible(true);
        mParameterLapLabel.setVisible(false);
        mParameterLapCombo.setVisible(false);

        mParameterXButton.setBounds(areaSpatParams.getTopLeft().getX() + 17,
                                    areaSpatParams.getTopLeft().getY() + 15,
                                    80,
                                    15);
        mParameterYButton.setBounds(mParameterXButton.getBounds().getTopLeft().getX(),
                                    mParameterXButton.getBounds().getBottom() + 5,
                                    80,
                                    15);
        mParameterZButton.setBounds(mParameterXButton.getBounds().getTopLeft().getX(),
                                    mParameterYButton.getBounds().getBottom() + 5,
                                    80,
                                    15);
        mParameterAzimuthOrXYSpanButton.setBounds(mParameterXButton.getBounds().getTopLeft().getX(),
                                                  mParameterZButton.getBounds().getBottom() + 5,
                                                  80,
                                                  15);
        mParameterElevationOrZSpanButton.setBounds(mParameterXButton.getBounds().getTopLeft().getX(),
                                                   mParameterAzimuthOrXYSpanButton.getBounds().getBottom() + 5,
                                                   80,
                                                   15);

        mParameterXDescriptorCombo.setBounds(mParameterXButton.getRight() + 5, areaSpatParams.getTopLeft().getY() + 15, 100, 15);
        mParameterYDescriptorCombo.setBounds(mParameterXButton.getRight() + 5,
                                   mParameterXDescriptorCombo.getBounds().getBottom() + 5,
                                   100,
                                   15);
        mParameterZDescriptorCombo.setBounds(mParameterXButton.getRight() + 5,
                                   mParameterYDescriptorCombo.getBounds().getBottom() + 5,
                                   100,
                                   15);
        mParameterAzimuthOrXYSpanDescriptorCombo.setBounds(mParameterXButton.getRight() + 5,
                                                 mParameterZDescriptorCombo.getBounds().getBottom() + 5,
                                                 100,
                                                 15);
        mParameterElevationOrZSpanDescriptorCombo.setBounds(mParameterXButton.getRight() + 5,
                                                  mParameterAzimuthOrXYSpanDescriptorCombo.getBounds().getBottom() + 5,
                                                  100,
                                                  15);

        mParameterRangeLabel.setBounds(mParameterXDescriptorCombo.getRight() + 3,
                                       areaSpatParams.getTopLeft().getY() + 2,
                                       40,
                                       15);

        mParameterXRangeSlider.setBounds(mParameterXDescriptorCombo.getRight() + 5,
                                         areaSpatParams.getTopLeft().getY() + 17,
                                         35,
                                         12);
        mParameterYRangeSlider.setBounds(mParameterXDescriptorCombo.getRight() + 5,
                                         mParameterXRangeSlider.getBounds().getBottomLeft().getY() + 8,
                                         35,
                                         12);
        mParameterZRangeSlider.setBounds(mParameterXDescriptorCombo.getRight() + 5,
                                         mParameterYRangeSlider.getBounds().getBottomLeft().getY() + 8,
                                         35,
                                         12);
        mParameterAzimuthOrXYSpanRangeSlider.setBounds(mParameterXDescriptorCombo.getRight() + 5,
                                                       mParameterZRangeSlider.getBounds().getBottomLeft().getY() + 8,
                                                       35,
                                                       12);
        mParameterElevationOrZSpanRangeSlider.setBounds(
            mParameterXDescriptorCombo.getRight() + 5,
            mParameterAzimuthOrXYSpanRangeSlider.getBounds().getBottomLeft().getY() + 8,
            35,
            12);

        mParameterOffsetLabel.setBounds(mParameterXRangeSlider.getRight() + 3,
                                        areaSpatParams.getTopLeft().getY() + 2,
                                        40,
                                        15);

        mParameterElevationZOffsetSlider.setBounds(mParameterXRangeSlider.getRight() + 5,
                                                   mParameterZRangeSlider.getBounds().getTopLeft().getY(),
                                                   35,
                                                   12);
        mParameterEleZSpanOffsetSlider.setBounds(mParameterXRangeSlider.getRight() + 5,
                                                 mParameterElevationOrZSpanRangeSlider.getBounds().getTopLeft().getY(),
                                                 35,
                                                 12);

        mParameterLapLabel.setBounds(mParameterOffsetLabel.getRight() + 7,
                                     areaSpatParams.getTopLeft().getY() + 2,
                                     40,
                                     15);

        mParameterLapCombo.setBounds(mParameterOffsetLabel.getRight() + 5,
                                     mParameterXDescriptorCombo.getBounds().getTopLeft().getY(),
                                     30,
                                     15);

        if (mXYParamLinked) {
            mParameterLapLabel.setVisible(true);
            mParameterLapCombo.setVisible(true);

            mParameterYButton.setEnabled(false);
            mParameterYDescriptorCombo.setEnabled(false);
            mParameterYRangeSlider.setEnabled(false);
        }
    }
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::mouseDown(juce::MouseEvent const & event)
{
    // Area where the XYLinked line is shown.
    juce::Rectangle<float> const xyLinkedLineArea{ 1.0f, 41.0f, 15.0f, 22.0f };
    if (xyLinkedLineArea.contains(event.getMouseDownPosition().toFloat())) {
        mXYParamLinked = !mXYParamLinked;
        repaint();
        resized();
    }
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::timerCallback()
{
    mOnsetDetectiontimerCounter += getTimerInterval();

    if (mOnsetDetectiontimerCounter >= 1000 * 120) {
        stopTimer();
        mOnsetDetectiontimerCounter = 0;
        mClickTimerButton.setButtonText(juce::String("click me!"));
    } else {
        if (mOnsetDetectiontimerCounter < 1000) {
            mClickTimerButton.setButtonText(juce::String(mOnsetDetectiontimerCounter) + juce::String(" ms"));
        } else {
            auto buttonTimerValue = static_cast<double>(mOnsetDetectiontimerCounter) / 1000;
            auto formattedTimerValue = juce::String(buttonTimerValue, 2);
            mClickTimerButton.setButtonText(formattedTimerValue + juce::String(" s"));
        }
    }
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::setSpatMode(SpatMode spatMode)
{
    mSpatMode = spatMode;

    if (mSpatMode == SpatMode::cube) {
        mParameterAzimuthOrXYSpanButton.setButtonText("X-Y Span");
        mParameterElevationOrZSpanButton.setButtonText("Z Span");
    } else {
        mParameterAzimuthOrXYSpanButton.setButtonText("Azimuth Span");
        mParameterElevationOrZSpanButton.setButtonText("Elevation Span");
    }

    resized();
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::refreshDescriptorPanel()
{
    mDataGraph.setSpatialParameter(mParameterToShow);
}


//==============================================================================
gris::DataGraph::DataGraph()
{
    mGUIBuffer.resize(100);
    std::fill(mGUIBuffer.begin(), mGUIBuffer.end(), 0.0);

    startTimer(50);
}

//==============================================================================
gris::DataGraph::~DataGraph()
{
}

//==============================================================================
void gris::DataGraph::paint(juce::Graphics & g)
{
    g.fillAll(juce::Colour(200, 200, 102));

    g.setOpacity(1.0f);
    g.setColour(juce::Colours::black);

    auto area = getLocalBounds().reduced(1);
    juce::RectangleList<float> rectList{};

    if (mParam) {
        auto & param{ mParam->get() };
        for (int i{}; i < mGUIBuffer.size(); ++i) {
            float initialX{}, initialY{}, width{}, height{};
            float valueToPaint{ static_cast<float>(mGUIBuffer.at(i)) };

            if (param.getParameterID() == ParameterID::elevation || param.getParameterID() == ParameterID::elevationspan
                || param.getParameterID() == ParameterID::z) {
                // parameter has an offset option, the graph can have negative values
                initialX = (static_cast<float>(area.getWidth()) / static_cast<float>(mGUIBuffer.size())) * (i + 1);
                width = static_cast<float>(area.getWidth()) / mGUIBuffer.size();
                height = static_cast<float>(area.getHeight() * std::abs(valueToPaint) / 2);
                if (valueToPaint < 0) {
                    // bottom half
                    initialY = static_cast<float>((area.getHeight() / 2) + 1.0f);
                    rectList.add(initialX, initialY, width, height);
                } else {
                    // top half
                    initialY = static_cast<float>((area.getHeight() / 2) - height + 1.0f);
                    rectList.add(initialX, initialY, width, height);
                }
            } else {
                // the graph uses only positive values
                initialX = (static_cast<float>(area.getWidth()) / static_cast<float>(mGUIBuffer.size())) * (i + 1);
                initialY = static_cast<float>(area.getHeight() - (area.getHeight() * std::abs(mGUIBuffer.at(i))) + 1);
                width = static_cast<float>(area.getWidth()) / mGUIBuffer.size();
                height = static_cast<float>(area.getHeight() * std::abs(mGUIBuffer.at(i)));
                rectList.add(initialX, initialY, width, height);
            }
        }
    }

    g.fillRectList(rectList);
}

//==============================================================================
void gris::DataGraph::resized()
{
}

//==============================================================================
void gris::DataGraph::timerCallback()
{
    if (isVisible()) {
        mGUIBuffer.push_back(readBufferMean());
        if (mGUIBuffer.size() > 100) {
            mGUIBuffer.pop_front();
        }
        repaint();
    }
}

//==============================================================================
void gris::DataGraph::addToBuffer(double value)
{
    mBuffer += value;
    mBufferCount++;
}

//==============================================================================
double gris::DataGraph::readBufferMean()
{
    double mean{};

    if (mBufferCount > 0) {
        mean = mBuffer / mBufferCount;
        mBuffer = 0.0;
        mBufferCount = 0;
    }

    return mean;
}

//==============================================================================
void gris::DataGraph::setSpatialParameter(std::optional<std::reference_wrapper<SpatialParameter>> param)
{
    mParam = param;
}
