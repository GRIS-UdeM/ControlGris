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

#include "cg_ElevationDrawingHandle.hpp"
#include "cg_ElevationSourceComponent.hpp"
#include "cg_GeneralSettingsTab.hpp"
#include "cg_PositionSourceComponent.hpp"
#include "cg_TrajectoryManager.hpp"

namespace gris
{
class Sources;
//==============================================================================
/** Base class for a component that displays a flatten view of the sources' positions.
 *
 * TODO : This whole UI system is a mess
 */
class FieldComponent
    : public juce::Component
    , public Source::Listener
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() noexcept = default;
        virtual ~Listener() noexcept = default;
        //==============================================================================
        Listener(Listener const &) = delete;
        Listener(Listener &&) = delete;
        Listener & operator=(Listener const &) = delete;
        Listener & operator=(Listener &&) = delete;
        //==============================================================================
        virtual void fieldSourcePositionChangedCallback(SourceIndex sourceId, int whichField) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)

    }; // class FieldComponent::Listener

    //==============================================================================
    juce::ListenerList<Listener> mListeners{};

protected:
    //==============================================================================
    Sources & mSources;
    bool mIsPlaying{ false };
    std::optional<SourceIndex> mSelectedSource{};
    std::optional<SourceIndex> mOldSelectedSource{};
    bool mDisplayInvalidSourceMoveWarning{};

public:
    //==============================================================================
    explicit FieldComponent(Sources & sources) noexcept;
    FieldComponent() = delete;
    ~FieldComponent() noexcept override;
    //==============================================================================
    FieldComponent(FieldComponent const &) = delete;
    FieldComponent(FieldComponent &&) = delete;
    FieldComponent & operator=(FieldComponent const &) = delete;
    FieldComponent & operator=(FieldComponent &&) = delete;
    //==============================================================================
    void refreshSources();
    void setSelectedSource(std::optional<SourceIndex> selectedSource);
    [[nodiscard]] auto const & getSelectedSourceIndex() const { return mSelectedSource; }
    void setIsPlaying(bool const state) { mIsPlaying = state; }
    [[nodiscard]] bool isPlaying() const { return mIsPlaying; }
    void addListener(Listener * l) { mListeners.add(l); }
    void displayInvalidSourceMoveWarning(bool state);
    [[nodiscard]] Sources & getSources() { return mSources; }
    [[nodiscard]] Sources const & getSources() const { return mSources; }
    //==============================================================================
    void paint(juce::Graphics & g) override;
    [[nodiscard]] virtual juce::Rectangle<float> getEffectiveArea() const = 0;

protected:
    //==============================================================================
    void drawBackgroundGrid(juce::Graphics & g) const;
    //==============================================================================
    void sourceMovedCallback() override;
    //==============================================================================
    virtual void drawBackground(juce::Graphics & g) const = 0;
    virtual void applySourceSelectionToComponents() = 0;
    virtual void notifySourcePositionChanged(SourceIndex sourceIndex) = 0;
    virtual void rebuildSourceComponents(int numberOfSources) = 0;
    virtual void drawSpans(juce::Graphics & g) const = 0;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(FieldComponent)

}; // class FieldComponent

//==============================================================================
/** A 2D view used to display the sources' [x,y] coordinates. */
class PositionFieldComponent final : public FieldComponent
{
    PositionTrajectoryManager & mPositionTrajectoryManager;
    SpatMode mSpatMode{ SpatMode::dome };
    std::optional<juce::Point<float>> mLineDrawingStartPosition{ std::nullopt };
    std::optional<juce::Point<float>> mLineDrawingEndPosition{ std::nullopt };
    SourceComponent mDrawingHandleComponent{ juce::Colour::fromRGB(176, 176, 228), "X" };
    juce::OwnedArray<PositionSourceComponent> mSourceComponents{};

public:
    //==============================================================================
    PositionFieldComponent() = delete;
    PositionFieldComponent(Sources & sources, PositionTrajectoryManager & positionAutomationManager) noexcept;
    ~PositionFieldComponent() noexcept override = default;
    //==============================================================================
    PositionFieldComponent(PositionFieldComponent const &) = delete;
    PositionFieldComponent(PositionFieldComponent &&) = delete;
    PositionFieldComponent & operator=(PositionFieldComponent const &) = delete;
    PositionFieldComponent & operator=(PositionFieldComponent &&) = delete;
    //==============================================================================
    [[nodiscard]] PositionTrajectoryManager const & getTrajectoryManager() const { return mPositionTrajectoryManager; }
    [[nodiscard]] PositionTrajectoryManager & getTrajectoryManager() { return mPositionTrajectoryManager; }
    void drawDomeSpans(juce::Graphics & g) const;
    void drawCubeSpans(juce::Graphics & g) const;
    void setSpatMode(SpatMode spatMode);
    [[nodiscard]] juce::Point<float> sourcePositionToComponentPosition(juce::Point<float> const & sourcePosition) const;
    [[nodiscard]] juce::Line<float> sourcePositionToComponentPosition(juce::Line<float> const & sourcePosition) const;
    [[nodiscard]] juce::Point<float>
        componentPositionToSourcePosition(juce::Point<float> const & componentPosition) const;
    //==============================================================================
    void drawSpans(juce::Graphics & g) const override;
    void paint(juce::Graphics & g) override;
    void resized() override;
    void rebuildSourceComponents(int numberOfSources) override;
    [[nodiscard]] juce::Rectangle<float> getEffectiveArea() const override;
    void notifySourcePositionChanged(SourceIndex sourceIndex) override;

private:
    //==============================================================================
    void mouseDown(juce::MouseEvent const & event) override;
    void mouseDrag(juce::MouseEvent const & event) override;
    void mouseUp(juce::MouseEvent const & event) override;
    void mouseMove(juce::MouseEvent const & event) override;
    void applySourceSelectionToComponents() override;
    void drawBackground(juce::Graphics & g) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionFieldComponent)

}; // class PositionFieldComponent

//==============================================================================
/** A 2D view used to display the sources' elevation (z coordinate). */
class ElevationFieldComponent final : public FieldComponent
{
    static constexpr auto TOP_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto BOTTOM_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto LEFT_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };
    static constexpr auto RIGHT_PADDING{ SOURCE_FIELD_COMPONENT_DIAMETER };

    ElevationTrajectoryManager & mElevationTrajectoryManager;
    ElevationDrawingHandle mDrawingHandle{ *this };
    juce::OwnedArray<ElevationSourceComponent> mSourceComponents{};

public:
    //==============================================================================
    ElevationFieldComponent() = delete;
    ElevationFieldComponent(Sources & sources, ElevationTrajectoryManager & positionTrajectoryManager) noexcept;
    ~ElevationFieldComponent() noexcept override = default;
    //==============================================================================
    ElevationFieldComponent(ElevationFieldComponent const &) = delete;
    ElevationFieldComponent(ElevationFieldComponent &&) = delete;
    ElevationFieldComponent & operator=(ElevationFieldComponent const &) = delete;
    ElevationFieldComponent & operator=(ElevationFieldComponent &&) = delete;
    //==============================================================================
    [[nodiscard]] ElevationTrajectoryManager const & getTrajectoryManager() const
    {
        return mElevationTrajectoryManager;
    }
    ElevationTrajectoryManager & getTrajectoryManager() { return mElevationTrajectoryManager; }
    [[nodiscard]] juce::Point<float> sourceElevationToComponentPosition(Radians sourceElevation,
                                                                        SourceIndex index) const;
    [[nodiscard]] Radians componentPositionToSourceElevation(juce::Point<float> const & componentPosition) const;
    //==============================================================================
    void paint(juce::Graphics & g) override;
    void resized() override;
    void mouseDown(juce::MouseEvent const & event) override;
    void mouseDrag(juce::MouseEvent const & event) override;
    void mouseUp(juce::MouseEvent const & event) override;
    void notifySourcePositionChanged(SourceIndex sourceIndex) override;
    void rebuildSourceComponents(int numberOfSources) override;
    [[nodiscard]] juce::Rectangle<float> getEffectiveArea() const override;

private:
    //==============================================================================
    void drawSpans(juce::Graphics & g) const override;
    void drawBackground(juce::Graphics & g) const override;
    void applySourceSelectionToComponents() override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationFieldComponent)

}; // class ElevationFieldComponent

} // namespace gris