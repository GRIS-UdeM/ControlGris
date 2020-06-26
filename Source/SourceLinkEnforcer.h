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
    SourceSnapshot() noexcept = default;
    SourceSnapshot(Source const & source) noexcept : position(source.getPos()), z(source.getElevation()) {}

    Point<float> position;
    Radians z; // height in CUBE mode, elevation in DOME mode
};

struct SourcesSnapshots {
    SourceSnapshot primary{};
    Array<SourceSnapshot> secondaries{};

    SourceSnapshot const & operator[](SourceIndex const index) const
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
        if (index.toInt() == 0) {
            return primary;
        }
        return secondaries.getReference(index.toInt() - 1);
    }
    SourceSnapshot & operator[](SourceIndex const index)
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
        if (index.toInt() == 0) {
            return primary;
        }
        return secondaries.getReference(index.toInt() - 1);
    }
    int size() const { return secondaries.size() + 1; }
};

class SourceLinkEnforcer : juce::ChangeListener
{
private:
    Sources & mSources;
    SourcesSnapshots mSnapshots{};
    AnySourceLink mSourceLink{};

public:
    SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink = PositionSourceLink::independent) noexcept;
    ~SourceLinkEnforcer() noexcept;

    void setSourceLink(AnySourceLink sourceLink);
    void numberOfSourcesChanged();
    void enforceSourceLink();

    auto const & getSnapshots() const { return mSnapshots; }
    void loadSnapshots(SourcesSnapshots const & snapshots);

private:
    void primarySourceMoved();
    void secondarySourceMoved(SourceIndex sourceIndex);
    void snapAll();
    void reset();

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceLinkEnforcer);
};
