/**************************************************************************
 * Copyright 2024 UdeM - GRIS - Gaël LANE LÉPINE                          *
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
 * <https://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#pragma once

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_constants.hpp"
#include "cg_NumSlider.h"

namespace gris
{
//==============================================================================
class SectionSoundReactiveSpatialization final : public juce::Component
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() = default;

        //virtual void futureVirtualFunctionsHere() = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners;

    SpatMode mSpatMode;

    // Audio anaylysis section
    juce::Label mAnalyzedSourceMixLabel;
    juce::ComboBox mAnalyzedSourceMixCombo;

    juce::Label mAudioAnalysisLabel;
    juce::ComboBox mAudioDescriptorCombo;

    juce::Label mDescriptorFactorLabel;
    juce::Label mDescriptorThresholdLabel;
    juce::Label mDescriptorMinFreqLabel;
    juce::Label mDescriptorMaxFreqLabel;
    juce::Label mDescriptorMinTimeLabel;
    juce::Label mDescriptorMaxTimeLabel;
    juce::Label mDescriptorSmoothLabel;
    juce::Label mDescriptorSmoothCoefLabel;

    juce::ComboBox mDescriptorMetricComboBox;

    NumSlider mDescriptorFactorSlider;
    NumSlider mDescriptorThresholdSlider;
    NumSlider mDescriptorMinFreqSlider;
    NumSlider mDescriptorMaxFreqSlider;
    NumSlider mDescriptorMinTimeSlider;
    NumSlider mDescriptorMaxTimeSlider;
    NumSlider mDescriptorSmoothSlider;
    NumSlider mDescriptorSmoothCoefSlider;

    juce::TextButton mClickTimerButton;

    // Spatial parameters section
    juce::Label mSpatialParameterLabel;

    juce::Label mParameterAzimuthLabel;
    juce::Label mParameterElevationLabel;
    juce::Label mParameterXLabel;
    juce::Label mParameterYLabel;
    juce::Label mParameterZLabel;
    juce::Label mParameterAzimuthOrXYSpanLabel;
    juce::Label mParameterElevationOrZSpanLabel;

    juce::Label mParameterAzimuthLapLabel;

    juce::Label mParameterElevationAndZOffsetLabel;
    juce::Label mParameterElevationAndZSpanOffsetLabel;

    juce::Label mParameterAzimuthRangeLabel;
    juce::Label mParameterElevationRangeLabel;
    juce::Label mParameterXRangeLabel;
    juce::Label mParameterYRangeLabel;
    juce::Label mParameterZRangeLabel;
    juce::Label mParameterAzimuthOrXYSpanRangeLabel;
    juce::Label mParameterElevationOrZSpanRangeLabel;

    juce::ComboBox mParameterAzimuthCombo;
    juce::ComboBox mParameterElevationCombo;
    juce::ComboBox mParameterXCombo;
    juce::ComboBox mParameterYCombo;
    juce::ComboBox mParameterZCombo;
    juce::ComboBox mParameterAzimuthOrXYSpanCombo;
    juce::ComboBox mParameterElevationOrZSpanCombo;

    NumSlider mDescriptorOffsetSlider;
    NumSlider mDescriptorRangeSlider;
    NumSlider mDescriptorLapSlider;
public:
    //==============================================================================
    explicit SectionSoundReactiveSpatialization(GrisLookAndFeel & grisLookAndFeel);
    //==============================================================================
    SectionSoundReactiveSpatialization() = delete;
    ~SectionSoundReactiveSpatialization() override = default;

    SectionSoundReactiveSpatialization(SectionSoundReactiveSpatialization const &) = delete;
    SectionSoundReactiveSpatialization(SectionSoundReactiveSpatialization &&) = delete;

    SectionSoundReactiveSpatialization & operator=(SectionSoundReactiveSpatialization const &) = delete;
    SectionSoundReactiveSpatialization & operator=(SectionSoundReactiveSpatialization &&) = delete;
    //==============================================================================

    //==============================================================================
    // overrides
    void paint(juce::Graphics & g) override;
    void resized() override;

    //==============================================================================
    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SectionSoundReactiveSpatialization)
};

} // namespace gris