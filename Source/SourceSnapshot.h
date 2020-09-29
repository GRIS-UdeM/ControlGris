#pragma once

#include <JuceHeader.h>

#include "StrongTypes.h"

class Source;

//==============================================================================
struct SourceSnapshot {
    Point<float> position;
    Radians z; // height in CUBE mode, elevation in DOME mode
    //==============================================================================
    SourceSnapshot() noexcept = default;
    ~SourceSnapshot() = default;

    SourceSnapshot(SourceSnapshot const &) = default;
    SourceSnapshot(SourceSnapshot &&) = default;

    SourceSnapshot & operator=(SourceSnapshot const &) = default;
    SourceSnapshot & operator=(SourceSnapshot &&) = default;
    //==============================================================================
    explicit SourceSnapshot(Source const & source) noexcept;

private:
    JUCE_LEAK_DETECTOR(SourceSnapshot)
}; // class SourceSnapshot

//==============================================================================
struct SourcesSnapshots {
    SourceSnapshot primary{};
    Array<SourceSnapshot> secondaries{};
    //==============================================================================
    SourceSnapshot const & operator[](SourceIndex const index) const;
    //==============================================================================
    SourceSnapshot & operator[](SourceIndex const index);
    int size() const { return secondaries.size() + 1; }

private:
    JUCE_LEAK_DETECTOR(SourceSnapshot)
}; // class SourcesSnapshots