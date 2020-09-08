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

#include <type_traits>

#include <JuceHeader.h>

#include "AutomationManager.h"
#include "ElevationDrawingHandle.h"
#include "ElevationSourceComponent.h"
#include "GrisLookAndFeel.h"
#include "PositionSourceComponent.h"
#include "SettingsBoxComponent.h"
#include "Source.h"

//==============================================================================
// This file defines the classes that implement the 2D view (azimuth-elevation
// or azimuth-distance) and the elevation view.
//
// Classes:
//   FieldComponent : The base class. Implements the common attributes and
//                    methods of the two views.
//   PositionFieldComponent : The 2D view as a cartesian plane. It is used to show
//                        and control two parameters, azimuth-elevation for the
//                        VBAP algorithm and azimuth-distance for the LBAP
//                        algorithm.
//   ElevationFieldComponent : The 1D view used to show and control the elevation
//                             parameter for the LBAP algorithm.
//==============================================================================

//==============================================================================
class FieldComponent
    : public Component
    , public ChangeListener
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() noexcept = default;
        virtual ~Listener() noexcept = default;

        Listener(Listener const &) = delete;
        Listener(Listener &&) = delete;

        Listener & operator=(Listener const &) = delete;
        Listener & operator=(Listener &&) = delete;
        //==============================================================================
        virtual void fieldSourcePositionChanged(SourceIndex sourceId, int whichField) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener);

    }; // class FieldComponent::Listener

    //==============================================================================
    ListenerList<Listener> mListeners{};

protected:
    //==============================================================================
    Sources & mSources;
    bool mIsPlaying{ false };
    std::optional<SourceIndex> mSelectedSource{};
    std::optional<SourceIndex> mOldSelectedSource{};

public:
    //==============================================================================
    FieldComponent() = delete;
    explicit FieldComponent(Sources & sources) noexcept;
    ~FieldComponent() noexcept override;

    FieldComponent(FieldComponent const &) = delete;
    FieldComponent(FieldComponent &&) = delete;

    FieldComponent & operator=(FieldComponent const &) = delete;
    FieldComponent & operator=(FieldComponent &&) = delete;
    //==============================================================================
    void refreshSources();
    void setSelectedSource(std::optional<SourceIndex> selectedSource);
    [[nodiscard]] auto const & getSelectedSourceIndex() const { return mSelectedSource; }
    void setIsPlaying(bool const state) { mIsPlaying = state; }
    void addListener(Listener * l) { mListeners.add(l); }
    //==============================================================================
    [[nodiscard]] virtual Rectangle<float> getEffectiveArea() const = 0;

protected:
    //==============================================================================
    void drawBackgroundGrid(Graphics & g) const;
    //==============================================================================
    virtual void drawBackground(Graphics & g) const = 0;
    virtual void applySourceSelectionToComponents() = 0;
    virtual void notifySourcePositionChanged(SourceIndex sourceIndex) = 0;
    virtual void rebuildSourceComponents(int numberOfSources) = 0;
    virtual void drawSpans(Graphics & g) const = 0;

private:
    //==============================================================================
    void changeListenerCallback(ChangeBroadcaster * broadcaster) final;
    //==============================================================================
    JUCE_LEAK_DETECTOR(FieldComponent)

}; // class FieldComponent

//==============================================================================
class PositionFieldComponent final : public FieldComponent
{
    PositionAutomationManager & mAutomationManager;
    SpatMode mSpatMode{ SpatMode::dome };
    bool mShowCircularSourceSelectionWarning{ false };
    std::optional<Point<float>> mLineDrawingStartPosition{ std::nullopt };
    std::optional<Point<float>> mLineDrawingEndPosition{ std::nullopt };
    SourceComponent mDrawingHandleComponent{ Colour::fromRGB(176, 176, 228), "X" };
    OwnedArray<PositionSourceComponent> mSourceComponents{};

public:
    //==============================================================================
    PositionFieldComponent() = delete;
    PositionFieldComponent(Sources & sources, PositionAutomationManager & positionAutomationManager) noexcept;
    ~PositionFieldComponent() noexcept final = default;

    PositionFieldComponent(PositionFieldComponent const &) = delete;
    PositionFieldComponent(PositionFieldComponent &&) = delete;

    PositionFieldComponent & operator=(PositionFieldComponent const &) = delete;
    PositionFieldComponent & operator=(PositionFieldComponent &&) = delete;
    //==============================================================================
    [[nodiscard]] PositionAutomationManager const & getAutomationManager() const { return mAutomationManager; }
    PositionAutomationManager & getAutomationManager() { return mAutomationManager; }

    void drawDomeSpans(Graphics & g) const;
    void drawCubeSpans(Graphics & g) const;
    void setSpatMode(SpatMode spatMode);

    void drawSpans(Graphics & g) const final;
    void paint(Graphics & g) final;
    void resized() final;
    void rebuildSourceComponents(int numberOfSources) final;
    [[nodiscard]] Rectangle<float> getEffectiveArea() const final;
    void notifySourcePositionChanged(SourceIndex sourceIndex) final;

    [[maybe_unused]] void setCircularSourceSelectionWarning(bool showCircularSourceSelectionWarning);

    [[nodiscard]] Point<float> sourcePositionToComponentPosition(Point<float> const & sourcePosition) const;
    [[nodiscard]] Line<float> sourcePositionToComponentPosition(Line<float> const & sourcePosition) const;
    [[nodiscard]] Point<float> componentPositionToSourcePosition(Point<float> const & componentPosition) const;

private:
    //==============================================================================
    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;
    void mouseMove(MouseEvent const & event) final;
    void applySourceSelectionToComponents() final;
    void drawBackground(Graphics & g) const final;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionFieldComponent)

}; // class PositionFieldComponent

//==============================================================================
class ElevationFieldComponent final : public FieldComponent
{
    static constexpr auto TOP_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto BOTTOM_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto LEFT_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto RIGHT_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };

    ElevationAutomationManager & mAutomationManager;
    ElevationDrawingHandle mDrawingHandle{ *this };
    OwnedArray<ElevationSourceComponent> mSourceComponents{};

public:
    //==============================================================================
    ElevationFieldComponent() = delete;
    ElevationFieldComponent(Sources & sources, ElevationAutomationManager & mPositionAutomationManager) noexcept;
    ~ElevationFieldComponent() noexcept final = default;

    ElevationFieldComponent(ElevationFieldComponent const &) = delete;
    ElevationFieldComponent(ElevationFieldComponent &&) = delete;

    ElevationFieldComponent & operator=(ElevationFieldComponent const &) = delete;
    ElevationFieldComponent & operator=(ElevationFieldComponent &&) = delete;
    //==============================================================================
    [[nodiscard]] ElevationAutomationManager const & getAutomationManager() const { return mAutomationManager; }
    ElevationAutomationManager & getAutomationManager() { return mAutomationManager; }
    [[nodiscard]] Point<float> sourceElevationToComponentPosition(Radians sourceElevation, SourceIndex index) const;
    [[nodiscard]] Radians componentPositionToSourceElevation(Point<float> const & componentPosition) const;
    //==============================================================================
    void paint(Graphics & g) final;
    void resized() final;
    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;
    void notifySourcePositionChanged(SourceIndex sourceIndex) final;
    void rebuildSourceComponents(int numberOfSources) final;
    [[nodiscard]] Rectangle<float> getEffectiveArea() const final;

private:
    //==============================================================================
    void drawSpans(Graphics & g) const final;
    void drawBackground(Graphics & g) const final;
    void applySourceSelectionToComponents() final;
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationFieldComponent)

}; // class ElevationFieldComponent
