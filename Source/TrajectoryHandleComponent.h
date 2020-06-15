/*
  ==============================================================================

    TrajectoryHandleComponent.h
    Created: 9 Jun 2020 2:40:16pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include <numeric>

#include "FieldComponentClickableItem.h"

class TrajectoryHandleComponent final : public FieldComponentClickableItem
{
private:
    FieldComponent & mFieldComponent;

public:
    static constexpr int SOURCE_ID = std::numeric_limits<int>::max();

    TrajectoryHandleComponent(FieldComponent & fieldComponent) noexcept;
    ~TrajectoryHandleComponent() noexcept final = default;

private:
    void mouseDown([[maybe_unused]] MouseEvent const & event) final{};
    void mouseDrag([[maybe_unused]] MouseEvent const & event) final{};
    void mouseUp([[maybe_unused]] MouseEvent const & event) final{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrajectoryHandleComponent);
};