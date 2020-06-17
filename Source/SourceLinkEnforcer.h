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

    void primarySourceMoved();
    void secondarySourceMoved(SourceIndex sourceIndex);
    void numberOfSourcesChanged();

private:
    void snapAll();
    void reset();

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceLinkEnforcer);
};

template<typename Derived>
class SourceLinkAlgo
{
    bool mInitialized{ false };

public:
    void calculateParams(SourceSnapshot const & primarySourceSnapshot)
    {
        static_cast<Derived *>(this)->calculateParams_impl(primarySourceSnapshot);
        mInitialized = true;
    }

    void apply(Array<SourceSnapshot> & secondarySnapshots) const
    {
        for (auto & snapshot : secondarySnapshots) {
            static_cast<Derived const *>(this)->apply(snapshot);
        }
    }

    void apply(SourceSnapshot & snapshot) const
    {
        jassert(mInitialized);
        static_cast<Derived const *>(this)->apply_impl(snapshot);
    }

    SourceSnapshot getInversedSnapshot(SourceSnapshot const & snapshot) const
    {
        jassert(mInitialized);
        return static_cast<Derived const *>(this)->getInversedSnapshot_impl(snapshot);
    }
};

class CircularLinkAlgo : public SourceLinkAlgo<CircularLinkAlgo>
{
private:
    Radians mRotation;
    float mRadiusRatio;

public:
    void calculateParams_impl(SourceSnapshot const & primarySourceSnapshot)
    {
        auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) };
        mRotation = primarySourceSnapshot.source->getAzimuth() - primarySourceSnapshot.azimuth;
        auto const primarySourceInitialRadius{ primarySourceSnapshot.position.getDistanceFromOrigin() };
        auto const radius{ primarySourceInitialRadius == 0.0f
                               ? notQuiteZero
                               : primarySourceSnapshot.source->getPos().getDistanceFromOrigin()
                                     / primarySourceInitialRadius };
        mRadiusRatio = radius == 0.0f ? notQuiteZero : radius;
    }

    void apply_impl(SourceSnapshot & snapshot) const
    {
        auto const newPosition{ snapshot.position.rotatedAboutOrigin(mRotation.getAsRadians()) * mRadiusRatio };
        snapshot.source->setPos(newPosition, SourceLinkNotification::silent);
    }

    SourceSnapshot getInversedSnapshot_impl(SourceSnapshot const & snapshot) const
    {
        SourceSnapshot result{ snapshot };
        auto const newPosition{
            (snapshot.source->getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians())
        };
        result.position = newPosition;
        return result;
    }
};