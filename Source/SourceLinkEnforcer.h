/*
  ==============================================================================

    SourceLinkEnforcer.h
    Created: 16 Jun 2020 2:21:04pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include <variant>

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "Source.h"

using AnySourceLink = std::variant<PositionSourceLink, ElevationSourceLink>;

struct SourceCoords {
    SourceCoords() noexcept = default;
    SourceCoords(Source const & source) noexcept
        : azimuth(source.getAzimuth())
        , elevation(source.getElevation())
        , distance(source.getDistance())
        , position(source.getPos())
    {
    }

    Radians azimuth{};
    Radians elevation{};
    float distance{}; // TODO: useful?
    Point<float> position{};
};

struct SourceLinkParameters {
    SourceCoords primarySourceInitialCoords{};
    Array<SourceCoords> secondarySourcesInitialCoords{};

    SourceCoords const & operator[](SourceIndex const index) const
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondarySourcesInitialCoords.size() + 1);
        if (index.toInt() == 0) {
            return primarySourceInitialCoords;
        }
        return secondarySourcesInitialCoords.getReference(index.toInt() - 1);
    }
    SourceCoords & operator[](SourceIndex const index)
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondarySourcesInitialCoords.size() + 1);
        if (index.toInt() == 0) {
            return primarySourceInitialCoords;
        }
        return secondarySourcesInitialCoords.getReference(index.toInt() - 1);
    }
    int size() const { return secondarySourcesInitialCoords.size() + 1; }
};

class SourceLinkEnforcer : juce::ChangeListener
{
private:
    Sources & mSources;
    SourceLinkParameters mParameters{};
    AnySourceLink mSourceLink{};

public:
    SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink = PositionSourceLink::independent) noexcept;
    ~SourceLinkEnforcer() noexcept;

    void setSourceLink(AnySourceLink sourceLink);
    void numberOfSourcesChanged();
    void enforceSourceLink();

    auto const & getParameters() const { return mParameters; }
    void loadParameters(SourceLinkParameters const & parameters);

private:
    void primarySourceMoved();
    void secondarySourceMoved(SourceIndex sourceIndex);
    void snapAll();
    void reset();

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceLinkEnforcer);
};
