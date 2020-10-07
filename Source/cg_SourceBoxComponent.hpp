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

#pragma once

#include <optional>

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_Source.hpp"
#include "cg_constants.hpp"

namespace gris
{
class SourceBoxComponent;

//==============================================================================
class DomeControls final : public juce::Component
{
    SourceBoxComponent & mSourceBoxComponent;

    Degrees mCurrentAzimuth;
    Radians mCurrentElevation;
    juce::Label mElevationLabel;
    juce::Label mAzimuthLabel;
    juce::Slider mElevationSlider;
    juce::Slider mAzimuthSlider;

public:
    //==============================================================================
    explicit DomeControls(SourceBoxComponent & sourceBoxComponent);
    ~DomeControls() override = default;

    DomeControls(DomeControls const &) = delete;
    DomeControls(DomeControls &&) = delete;

    DomeControls & operator=(DomeControls const &) = delete;
    DomeControls & operator=(DomeControls &&) = delete;
    //==============================================================================
    void updateSliderValues(Source * source);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(DomeControls)
}; // DomeControls

//==============================================================================
class CubeControls final : public juce::Component
{
    SourceBoxComponent & mSourceBoxComponent;

    float mCurrentX;
    float mCurrentY;
    float mCurrentZ;
    juce::Label mXLabel;
    juce::Label mYLabel;
    juce::Label mZLabel;
    juce::Slider mXSlider;
    juce::Slider mYSlider;
    juce::Slider mZSlider;

public:
    //==============================================================================
    explicit CubeControls(SourceBoxComponent & sourceBoxComponent);
    ~CubeControls() override = default;

    CubeControls(CubeControls const &) = delete;
    CubeControls(CubeControls &&) = delete;

    CubeControls & operator=(CubeControls const &) = delete;
    CubeControls & operator=(CubeControls &&) = delete;
    //==============================================================================
    void updateSliderValues(Source * source);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(CubeControls)
}; // CubeControls

//==============================================================================
class SourceBoxComponent final : public juce::Component
{
    friend DomeControls;
    friend CubeControls;

public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() = default;

        virtual void sourceBoxPlacementChanged(SourcePlacement value) = 0;
        virtual void sourceBoxSelectionChanged(SourceIndex sourceIndex) = 0;
        virtual void sourceBoxPositionChanged(SourceIndex sourceIndex,
                                              std::optional<Radians> azimuth,
                                              std::optional<Radians> elevation,
                                              std::optional<float> x,
                                              std::optional<float> y,
                                              std::optional<float> z)
            = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners;

    SourceIndex mSelectedSource;

    juce::Label mSourcePlacementLabel;
    juce::ComboBox mSourcePlacementCombo;

    juce::Label mSourceNumberLabel;
    juce::ComboBox mSourceNumberCombo;

    DomeControls mDomeControls;
    CubeControls mCubeControls;

public:
    //==============================================================================
    explicit SourceBoxComponent(GrisLookAndFeel & grisLookAndFeel, SpatMode spatMode);
    //==============================================================================
    SourceBoxComponent() = delete;
    ~SourceBoxComponent() override = default;

    SourceBoxComponent(SourceBoxComponent const &) = delete;
    SourceBoxComponent(SourceBoxComponent &&) = delete;

    SourceBoxComponent & operator=(SourceBoxComponent const &) = delete;
    SourceBoxComponent & operator=(SourceBoxComponent &&) = delete;
    //==============================================================================
    void setNumberOfSources(int numOfSources, SourceId firstSourceId);
    void updateSelectedSource(Source * source, SourceIndex sourceIndex, SpatMode spatMode);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

    void setSpatMode(SpatMode spatMode);
    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SourceBoxComponent)
};

} // namespace gris