/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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

#pragma once

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_Source.hpp"

namespace gris
{
class ControlGrisAudioProcessor;
class ControlGrisAudioProcessorEditor;
//==============================================================================
class SectionSourceSpan final : public juce::Component
{
    //==============================================================================
    ControlGrisAudioProcessor & mAudioProcessor;
    ControlGrisAudioProcessorEditor & mAudioProcessorEditor;
    GrisLookAndFeel & mGrisLookAndFeel;

    bool mDistanceEnabled{ false };
    bool mSpanLinked{ false };

    Source * mSelectedSource{};

    juce::Label mAzimuthLabel{};
    juce::Label mElevationLabel{};
    juce::Slider mAzimuthSpan{};
    juce::Slider mElevationSpan{};

    juce::AudioProcessorValueTreeState::SliderAttachment mAzimuthSliderAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment mElevationSliderAttachment;

public:
    //==============================================================================
    SectionSourceSpan(ControlGrisAudioProcessor & audioProcessor,
                      ControlGrisAudioProcessorEditor & audioProcessorEditor,
                      GrisLookAndFeel & grisLookAndFeel);
    //==============================================================================
    SectionSourceSpan() = delete;
    ~SectionSourceSpan() override = default;

    SectionSourceSpan(SectionSourceSpan const &) = delete;
    SectionSourceSpan(SectionSourceSpan &&) = delete;

    SectionSourceSpan & operator=(SectionSourceSpan const &) = delete;
    SectionSourceSpan & operator=(SectionSourceSpan &&) = delete;
    //==============================================================================
    void mouseDown(juce::MouseEvent const & event) override;
    void paint(juce::Graphics &) override;
    void resized() override;

    void setSelectedSource(Source * source);
    void setSpanLinkState(bool spanLinkState);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SectionSourceSpan)
};

} // namespace gris