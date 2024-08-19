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

#pragma once

#include <JuceHeader.h>

#include "cg_ControlGrisAudioProcessor.hpp"
#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_constants.hpp"
#include "cg_NumSlider.h"
#include "SpatialParameters/cg_SpatialParameter.h"

namespace gris
{
//==============================================================================
class DataGraph
    : public juce::Component
    , private juce::Timer
{
public:
    //==============================================================================
    DataGraph();
    ~DataGraph() override;

    void paint(juce::Graphics &) override;
    void resized() override;
    void timerCallback() override;

    void addToBuffer(double value);
    double readBufferMean();
    void setSpatialParameter(std::optional<std::reference_wrapper<SpatialParameter>> param);

private:
    //==============================================================================
    std::optional<std::reference_wrapper<SpatialParameter>> mParam;
    std::deque<double> mGUIBuffer;
    double mBuffer{};
    int mBufferCount{};

    //==============================================================================
    JUCE_LEAK_DETECTOR(DataGraph)
}; // class DataGraph

//==============================================================================
class SectionSoundReactiveSpatialization final
    : public juce::Component
    , private juce::Timer
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
    ControlGrisAudioProcessor & mAudioProcessor;

    juce::ListenerList<Listener> mListeners;
    SpatMode mSpatMode;

    //==============================================================================
    // Spatial parameters section
    bool mXYParamLinked{};

    juce::Label mSpatialParameterLabel;

    juce::TextButton mParameterAzimuthButton;
    juce::TextButton mParameterElevationButton;
    juce::TextButton mParameterXButton;
    juce::TextButton mParameterYButton;
    juce::TextButton mParameterZButton;
    juce::TextButton mParameterAzimuthOrXYSpanButton;
    juce::TextButton mParameterElevationOrZSpanButton;

    juce::ComboBox mParameterAzimuthDescriptorCombo;
    juce::ComboBox mParameterElevationDescriptorCombo;
    juce::ComboBox mParameterXDescriptorCombo;
    juce::ComboBox mParameterYDescriptorCombo;
    juce::ComboBox mParameterZDescriptorCombo;
    juce::ComboBox mParameterAzimuthOrXYSpanDescriptorCombo;
    juce::ComboBox mParameterElevationOrZSpanDescriptorCombo;

    juce::Label mParameterRangeLabel;

    NumSlider mParameterAzimuthRangeSlider;
    NumSlider mParameterElevationRangeSlider;
    NumSlider mParameterXRangeSlider;
    NumSlider mParameterYRangeSlider;
    NumSlider mParameterZRangeSlider;
    NumSlider mParameterAzimuthOrXYSpanRangeSlider;
    NumSlider mParameterElevationOrZSpanRangeSlider;

    juce::Label mParameterOffsetLabel;

    NumSlider mParameterElevationZOffsetSlider;
    NumSlider mParameterEleZSpanOffsetSlider;
    
    juce::Label mParameterLapLabel;
    //NumSlider mParameterLapSlider;
    juce::ComboBox mParameterLapCombo;

    //==============================================================================
    // Audio anaylysis section
    juce::Label mAudioAnalysisLabel;
    std::optional<std::reference_wrapper<SpatialParameter>> mParameterToShow;
    DataGraph mDataGraph;
    //juce::ComboBox mAudioDescriptorCombo;

    juce::Label mAnalyzedSourceMixLabel;
    juce::ComboBox mAnalyzedSourceMixCombo;

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
    int mOnsetDetectiontimerCounter{};

public:
    //==============================================================================
    explicit SectionSoundReactiveSpatialization(GrisLookAndFeel & grisLookAndFeel,
                                                ControlGrisAudioProcessor & audioProcessor);
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
    void mouseDown(juce::MouseEvent const & event) override;
    void timerCallback() override;

    //==============================================================================
    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

    void setSpatMode(SpatMode spatMode);

private:
    //==============================================================================
    void refreshDescriptorPanel();

    //==============================================================================
    JUCE_LEAK_DETECTOR(SectionSoundReactiveSpatialization)
};

} // namespace gris