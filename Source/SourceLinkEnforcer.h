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

struct SourceSnapshot {
    Source * source;
    Radians azimuth;
    Radians elevation;
    float distance; // TODO: useful?
    Point<float> position;

    void takeSnapshot()
    {
        azimuth = source->getAzimuth();
        elevation = source->getElevation();
        distance = source->getDistance();
        position = source->getPos();
    }
};

class SourceLinkEnforcer : juce::ChangeListener
{
private:
    Sources & mSources;
    SourceSnapshot mPrimarySourceSnapshot;
    Array<SourceSnapshot> mSecondarySourcesSnapshots;
    AnySourceLink mSourceLink;

public:
    SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink = PositionSourceLink::independent) noexcept;
    ~SourceLinkEnforcer() noexcept;

    void setSourceLink(AnySourceLink sourceLink);
    void numberOfSourcesChanged();
    void enforceSourceLink();

private:
    void primarySourceMoved();
    void secondarySourceMoved(SourceIndex sourceIndex);
    void snapAll();
    void reset();

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceLinkEnforcer);
};
