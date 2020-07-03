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

#include <array>

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "DumbOptional.h"
#include "SourceLinkEnforcer.h"

class PresetsManager final
    : public ChangeListener
    , public ChangeBroadcaster
{
private:
    int mLastLoadedPreset{ 0 };
    bool mSourceMovedSinceLastRecall{ false };
    XmlElement & mData;
    Sources & mSources;
    SourceLinkEnforcer & mPositionLinkEnforcer;
    SourceLinkEnforcer & mElevationLinkEnforcer;

public:
    PresetsManager(XmlElement & data,
                   Sources & sources,
                   SourceLinkEnforcer & positionLinkEnforcer,
                   SourceLinkEnforcer & elevationLinkEnforcer);
    ~PresetsManager() noexcept final;

    int getCurrentPreset() const { return mLastLoadedPreset; }

    bool loadIfPresetChanged(int presetNumber);
    bool forceLoad(int presetNumber);
    void save(int presetNumber);
    bool deletePreset(int presetNumber);

    std::array<bool, NUMBER_OF_POSITION_PRESETS> getSavedPresets() const;

private:
    [[nodiscard]] bool contains(int presetNumber) const;
    [[nodiscard]] std::unique_ptr<XmlElement> createPresetData(int presetNumber) const;
    [[nodiscard]] optional<XmlElement *> getPresetData(int presetNumber) const;

    [[nodiscard]] bool load(int presetNumber);
    void subscribeToSources();

    void unsubscribeToSources();

    void changeListenerCallback(ChangeBroadcaster * broadcaster) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetsManager);
};