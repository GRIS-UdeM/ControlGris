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

#include "cg_SourceBoxComponent.hpp"

#include "cg_constants.hpp"

//==============================================================================
DomeControls::DomeControls(SourceBoxComponent & sourceBoxComponent) : mSourceBoxComponent(sourceBoxComponent)
{
    mCurrentAzimuth = {};
    mCurrentElevation = MAX_ELEVATION;

    mElevationLabel.setText("Elevation:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mElevationLabel);

    mElevationSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.01f));
    mElevationSlider.setValue(1.0, NotificationType::dontSendNotification);
    mElevationSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    mElevationSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&mElevationSlider);
    mElevationSlider.onValueChange = [this] {
        mCurrentElevation = MAX_ELEVATION * (1.0f - mElevationSlider.getValue());
        mSourceBoxComponent.mListeners.call([&](SourceBoxComponent::Listener & l) {
            l.sourceBoxPositionChanged(mSourceBoxComponent.mSelectedSource,
                                       std::nullopt,
                                       mCurrentElevation,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt);
        });
    };

    mAzimuthLabel.setText("Azimuth:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mAzimuthLabel);

    mAzimuthSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 360.0f, 0.01f));
    mAzimuthSlider.setValue(0.0, NotificationType::dontSendNotification);
    mAzimuthSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    mAzimuthSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&mAzimuthSlider);
    mAzimuthSlider.onValueChange = [this] {
        mCurrentAzimuth = Degrees{ static_cast<float>(mAzimuthSlider.getValue()) };
        mSourceBoxComponent.mListeners.call([&](SourceBoxComponent::Listener & l) {
            l.sourceBoxPositionChanged(mSourceBoxComponent.mSelectedSource,
                                       mCurrentAzimuth,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt);
        });
    };

    mElevationLabel.setBounds(0, 0, 150, 15);
    mElevationSlider.setBounds(75, 0, 200, 20);

    mAzimuthLabel.setBounds(0, 30, 150, 15);
    mAzimuthSlider.setBounds(75, 30, 200, 20);
}

//==============================================================================
void DomeControls::updateSliderValues(Source * source)
{
    mCurrentAzimuth = source->getAzimuth();
    mCurrentElevation = MAX_ELEVATION * source->getNormalizedElevation().toFloat();

    if (mCurrentAzimuth.getAsDegrees() < 0.0f) {
        mCurrentAzimuth += Degrees{ 360.0f };
    }
    mAzimuthSlider.setValue(mCurrentAzimuth.getAsDegrees(), NotificationType::dontSendNotification);
    mElevationSlider.setValue(1.0f - mCurrentElevation / MAX_ELEVATION, NotificationType::dontSendNotification);
}

//==============================================================================
CubeControls::CubeControls(SourceBoxComponent & sourceBoxComponent) : mSourceBoxComponent(sourceBoxComponent)
{
    mCurrentX = { 0.0f };
    mCurrentY = { 0.0f };
    mCurrentZ = { 0.0f };

    mXLabel.setText("X", NotificationType::dontSendNotification);
    mYLabel.setText("Y", NotificationType::dontSendNotification);
    mZLabel.setText("Z", NotificationType::dontSendNotification);

    addAndMakeVisible(&mXLabel);
    addAndMakeVisible(&mYLabel);
    addAndMakeVisible(&mZLabel);

    mXSlider.setNormalisableRange(NormalisableRange<double>(-1.0f, 1.0f, 0.01f));
    mYSlider.setNormalisableRange(NormalisableRange<double>(-1.0f, 1.0f, 0.01f));
    mZSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.01f));

    mXSlider.setValue(0.0, NotificationType::dontSendNotification);
    mYSlider.setValue(0.0, NotificationType::dontSendNotification);
    mZSlider.setValue(0.0, NotificationType::dontSendNotification);

    mXSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 15);
    mYSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 15);
    mZSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 15);

    mXSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    mYSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    mZSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);

    addAndMakeVisible(&mXSlider);
    addAndMakeVisible(&mYSlider);
    addAndMakeVisible(&mZSlider);

    mXSlider.onValueChange = [this] {
        mCurrentX = mXSlider.getValue();
        mSourceBoxComponent.mListeners.call([&](SourceBoxComponent::Listener & l) {
            l.sourceBoxPositionChanged(mSourceBoxComponent.mSelectedSource,
                                       std::nullopt,
                                       std::nullopt,
                                       mCurrentX,
                                       std::nullopt,
                                       std::nullopt);
        });
    };
    mYSlider.onValueChange = [this] {
        mCurrentY = mYSlider.getValue();
        mSourceBoxComponent.mListeners.call([&](SourceBoxComponent::Listener & l) {
            l.sourceBoxPositionChanged(mSourceBoxComponent.mSelectedSource,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt,
                                       mCurrentY,
                                       std::nullopt);
        });
    };
    mZSlider.onValueChange = [this] {
        mCurrentZ = mZSlider.getValue();
        mSourceBoxComponent.mListeners.call([&](SourceBoxComponent::Listener & l) {
            l.sourceBoxPositionChanged(mSourceBoxComponent.mSelectedSource,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt,
                                       std::nullopt,
                                       1.0f - mCurrentZ);
        });
    };

    mXLabel.setBounds(0, 0, 150, 15);
    mYLabel.setBounds(0, 20, 150, 15);
    mZLabel.setBounds(0, 40, 150, 15);

    mXSlider.setBounds(75, 0, 200, 20);
    mYSlider.setBounds(75, 20, 200, 20);
    mZSlider.setBounds(75, 40, 200, 20);
}

//==============================================================================
void CubeControls::updateSliderValues(Source * source)
{
    mCurrentX = source->getX();
    mCurrentY = source->getY();
    mCurrentZ = 1.0f - source->getElevation() / MAX_ELEVATION;

    mXSlider.setValue(mCurrentX, NotificationType::dontSendNotification);
    mYSlider.setValue(mCurrentY, NotificationType::dontSendNotification);
    mZSlider.setValue(mCurrentZ, NotificationType::dontSendNotification);
}

//==============================================================================
SourceBoxComponent::SourceBoxComponent(GrisLookAndFeel & grisLookAndFeel, SpatMode const spatMode)
    : mGrisLookAndFeel(grisLookAndFeel)
    , mDomeControls(*this)
    , mCubeControls(*this)
{
    mSelectedSource = SourceIndex{};

    mSourcePlacementLabel.setText("Source Placement:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mSourcePlacementLabel);

    addAndMakeVisible(&mSourcePlacementCombo);
    mSourcePlacementCombo.setTextWhenNothingSelected("Choose a source placement...");
    mSourcePlacementCombo.addItemList(SOURCE_PLACEMENT_SKETCH, 1);
    mSourcePlacementCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.sourceBoxPlacementChanged(static_cast<SourcePlacement>(mSourcePlacementCombo.getSelectedId()));
            mSourcePlacementCombo.setSelectedId(0, NotificationType::dontSendNotification);
        });
    };

    mSourceNumberLabel.setText("Source Number:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mSourceNumberLabel);

    addAndMakeVisible(&mSourceNumberCombo);
    mSourceNumberCombo.setTextWhenNothingSelected("Choose a source...");
    for (auto i{ 1 }; i <= 8; ++i) {
        mSourceNumberCombo.addItem(juce::String{ i }, i);
    }
    mSourceNumberCombo.setSelectedId(mSelectedSource.toInt());
    mSourceNumberCombo.onChange = [this] {
        mSelectedSource = SourceIndex{ mSourceNumberCombo.getSelectedItemIndex() };
        mListeners.call([&](Listener & l) { l.sourceBoxSelectionChanged(mSelectedSource); });
    };

    addAndMakeVisible(&mDomeControls);
    addAndMakeVisible(&mCubeControls);
    setSpatMode(spatMode);
}

//==============================================================================
void SourceBoxComponent::paint(Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(ResizableWindow::backgroundColourId));
}

//==============================================================================
void SourceBoxComponent::resized()
{
    mSourcePlacementLabel.setBounds(5, 10, 150, 15);
    mSourcePlacementCombo.setBounds(130, 10, 150, 20);

    mSourceNumberLabel.setBounds(305, 10, 150, 15);
    mSourceNumberCombo.setBounds(430, 10, 150, 20);

    mDomeControls.setBounds(305, 40, 275, 500);
    mCubeControls.setBounds(305, 35, 275, 550);
}

//==============================================================================
void SourceBoxComponent::setNumberOfSources(int const numOfSources, SourceId const firstSourceId)
{
    mSourceNumberCombo.clear();
    for (auto id = firstSourceId; id < firstSourceId + numOfSources; ++id) {
        mSourceNumberCombo.addItem(id.toString(), id.toInt());
    }
    if (mSelectedSource >= SourceIndex{ numOfSources })
        mSelectedSource = SourceIndex{ 0 };
    mSourceNumberCombo.setSelectedItemIndex(mSelectedSource.toInt());
}

//==============================================================================
void SourceBoxComponent::updateSelectedSource(Source * source, SourceIndex const sourceIndex, SpatMode spatMode)
{
    mSelectedSource = sourceIndex;
    mSourceNumberCombo.setSelectedItemIndex(mSelectedSource.toInt());
    mDomeControls.updateSliderValues(source);
    mCubeControls.updateSliderValues(source);
}

void SourceBoxComponent::setSpatMode(SpatMode const spatMode)
{
    switch (spatMode) {
    case SpatMode::dome:
        mDomeControls.setVisible(true);
        mCubeControls.setVisible(false);
        break;
    case SpatMode::cube:
        mDomeControls.setVisible(false);
        mCubeControls.setVisible(true);
        break;
    default:
        jassertfalse;
    }
}