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
#include "SourceBoxComponent.h"

#include "ControlGrisConstants.h"

SourceBoxComponent::SourceBoxComponent()
{
    mSelectedSource = SourceIndex{};
    mCurrentAngle = {};
    mCurrentRayLength = 1.0f;

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
    for (int i{ 1 }; i <= 8; ++i) {
        mSourceNumberCombo.addItem(String(i), i);
    }
    mSourceNumberCombo.setSelectedId(mSelectedSource.toInt());
    mSourceNumberCombo.onChange = [this] {
        mSelectedSource = SourceIndex{ mSourceNumberCombo.getSelectedItemIndex() };
        mListeners.call([&](Listener & l) { l.sourceBoxSelectionChanged(mSelectedSource); });
    };

    mRayLengthLabel.setText("Ray Length:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mRayLengthLabel);

    mRayLengthSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.01f));
    mRayLengthSlider.setValue(1.0, NotificationType::dontSendNotification);
    mRayLengthSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    mRayLengthSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&mRayLengthSlider);
    mRayLengthSlider.onValueChange = [this] {
        mCurrentRayLength = (float)mRayLengthSlider.getValue();
        mListeners.call(
            [&](Listener & l) { l.sourceBoxPositionChanged(mSelectedSource, mCurrentAngle, mCurrentRayLength); });
    };

    mAngleLabel.setText("Angle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mAngleLabel);

    mAngleSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 360.0f, 0.01f));
    mAngleSlider.setValue(0.0, NotificationType::dontSendNotification);
    mAngleSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    mAngleSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&mAngleSlider);
    mAngleSlider.onValueChange = [this] {
        mCurrentAngle = Degrees{ static_cast<float>(mAngleSlider.getValue()) };
        mListeners.call(
            [&](Listener & l) { l.sourceBoxPositionChanged(mSelectedSource, mCurrentAngle, mCurrentRayLength); });
    };
}

SourceBoxComponent::~SourceBoxComponent()
{
    setLookAndFeel(nullptr);
}

void SourceBoxComponent::paint(Graphics & g)
{
    GrisLookAndFeel * lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *>(&getLookAndFeel());
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void SourceBoxComponent::resized()
{
    mSourcePlacementLabel.setBounds(5, 10, 150, 15);
    mSourcePlacementCombo.setBounds(130, 10, 150, 20);

    mSourceNumberLabel.setBounds(305, 10, 150, 15);
    mSourceNumberCombo.setBounds(430, 10, 150, 20);

    mRayLengthLabel.setBounds(305, 40, 150, 15);
    mRayLengthSlider.setBounds(380, 40, 200, 20);

    mAngleLabel.setBounds(305, 70, 150, 15);
    mAngleSlider.setBounds(380, 70, 200, 20);
}

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

void SourceBoxComponent::updateSelectedSource(Source * source, SourceIndex const sourceIndex, SpatMode spatMode)
{
    mSelectedSource = sourceIndex;
    mSourceNumberCombo.setSelectedItemIndex(mSelectedSource.toInt());
    if (spatMode == SpatMode::dome) {
        mCurrentAngle = source->getAzimuth();
        mCurrentRayLength = source->getNormalizedElevation();
    } else {
        mCurrentAngle = source->getAzimuth();
        mCurrentRayLength = source->getDistance();
    }
    if (mCurrentAngle.getAsDegrees() < 0.0f) {
        mCurrentAngle += Degrees{ 360.0f };
    }
    mAngleSlider.setValue(mCurrentAngle.getAsDegrees(), NotificationType::dontSendNotification);
    mRayLengthSlider.setValue(mCurrentRayLength, NotificationType::dontSendNotification);
}
