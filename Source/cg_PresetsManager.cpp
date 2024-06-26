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

#include "cg_PresetsManager.hpp"

#include "cg_utilities.hpp"

namespace gris
{
//==============================================================================
juce::String
    getFixedPosSourceName(FixedPositionType const fixedPositionType, SourceIndex const index, int const dimension)
{
    juce::String const type{ fixedPositionType == FixedPositionType::terminal ? "_terminal" : "" };
    juce::String result{};
    switch (dimension) {
    case 0:
        result = juce::String("S") + juce::String(index.get() + 1) + type + juce::String("_X");
        break;
    case 1:
        result = juce::String("S") + juce::String(index.get() + 1) + type + juce::String("_Y");
        break;
    case 2:
        result = juce::String("S") + juce::String(index.get() + 1) + type + juce::String("_Z");
        break;
    default:
        jassertfalse; // how did you get there?
    }
    return result;
}

//==============================================================================
PresetsManager::PresetsManager(juce::XmlElement & data,
                               Sources & sources,
                               SourceLinkEnforcer & positionLinkEnforcer,
                               SourceLinkEnforcer & elevationLinkEnforcer)
    : mData(data)
    , mSources(sources)
    , mPositionLinkEnforcer(positionLinkEnforcer)
    , mElevationLinkEnforcer(elevationLinkEnforcer)
{
}

//==============================================================================
int PresetsManager::getCurrentPreset() const
{
    return mLastLoadedPreset;
}

//==============================================================================
bool PresetsManager::loadIfPresetChanged(int const presetNumber)
{
    if (presetNumber == mLastLoadedPreset) {
        return false;
    }

    return load(presetNumber);
}

//==============================================================================
bool PresetsManager::forceLoad(int const presetNumber)
{
    return load(presetNumber);
}

//==============================================================================
bool PresetsManager::load(int const presetNumber)
{
    if (presetNumber != 0) {
        auto const maybe_presetData{ getPresetData(presetNumber) };
        if (!maybe_presetData.has_value()) {
            return false;
        }

        auto const * presetData{ *maybe_presetData };

        SourcesSnapshots snapshots{};
        for (auto & source : mSources) {
            SourceSnapshot snapshot{};
            auto const index{ source.getIndex() };
            auto const xPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 0) };
            auto const yPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 1) };
            if (presetData->hasAttribute(xPosId) && presetData->hasAttribute(yPosId)) {
                juce::Point<float> const normalizedInversedPosition{
                    static_cast<float>(presetData->getDoubleAttribute(xPosId)),
                    static_cast<float>(presetData->getDoubleAttribute(yPosId))
                };
                auto const inversedPosition{ normalizedInversedPosition * 2.0f - juce::Point<float>{ 1.0f, 1.0f } };
                juce::Point<float> const position{ inversedPosition.getX(), inversedPosition.getY() * -1.0f };
                snapshot.position = position;
                auto const zPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 2) };
                if (presetData->hasAttribute(zPosId)) {
                    auto const inversedNormalizedElevation{ static_cast<float>(
                        presetData->getDoubleAttribute(getFixedPosSourceName(FixedPositionType::initial, index, 2))) };
                    snapshot.z = MAX_ELEVATION * (1.0f - inversedNormalizedElevation);
                }
            }
            if (source.isPrimarySource()) {
                snapshots.primary = snapshot;
            } else {
                snapshots.secondaries.add(snapshot);
            }
        }

        mPositionLinkEnforcer.loadSnapshots(snapshots);
        if (mSources.getPrimarySource().getSpatMode() == SpatMode::cube) {
            mElevationLinkEnforcer.loadSnapshots(snapshots);
        }

        auto const xTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 0) };
        auto const yTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 1) };
        auto const zTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 2) };

        juce::Point<float> terminalPosition;
        if (presetData->hasAttribute(xTerminalPositionId) && presetData->hasAttribute(yTerminalPositionId)) {
            juce::Point<float> const inversedNormalizedTerminalPosition{
                static_cast<float>(presetData->getDoubleAttribute(xTerminalPositionId)),
                static_cast<float>(presetData->getDoubleAttribute(yTerminalPositionId))
            };
            auto const inversedTerminalPosition{ inversedNormalizedTerminalPosition * 2.0f
                                                 - juce::Point<float>{ 1.0f, 1.0f } };
            terminalPosition
                = juce::Point<float>{ inversedTerminalPosition.getX(), inversedTerminalPosition.getY() * -1.0f };
        } else {
            terminalPosition = snapshots.primary.position;
        }
        mSources.getPrimarySource().setPosition(terminalPosition, Source::OriginOfChange::presetRecall);

        if (mSources.getPrimarySource().getSpatMode() == SpatMode::cube) {
            Radians elevation;
            if (presetData->hasAttribute(zTerminalPositionId)) {
                auto const inversedNormalizedTerminalElevation{ static_cast<float>(
                    presetData->getDoubleAttribute(zTerminalPositionId)) };
                elevation = MAX_ELEVATION * (1.0f - inversedNormalizedTerminalElevation);
            } else {
                elevation = snapshots.primary.z;
            };

            mSources.getPrimarySource().setElevation(elevation, Source::OriginOfChange::presetRecall);
        }
    }
    mLastLoadedPreset = presetNumber;
    sendChangeMessage();

    return true;
}

//==============================================================================
bool PresetsManager::contains(int const presetNumber) const
{
    auto const presetData{ getPresetData(presetNumber) };
    return presetData.has_value();
}

//==============================================================================
std::optional<juce::XmlElement *> PresetsManager::getPresetData(int const presetNumber) const
{
    for (auto * element : mData.getChildIterator()) {
        if (element->getIntAttribute("ID") == presetNumber) {
            return element;
        }
    }

    return std::nullopt;
}

//==============================================================================
std::unique_ptr<juce::XmlElement> PresetsManager::createPresetData(int const presetNumber) const
{
    // Build a new fixed position element.
    auto result{ std::make_unique<juce::XmlElement>("ITEM") };
    result->setAttribute("ID", presetNumber);

    auto const & positionSnapshots{ mPositionLinkEnforcer.getSnapshots() };
    auto const & elevationsSnapshots{ mElevationLinkEnforcer.getSnapshots() };

    SourceIndex const numberOfSources{ mSources.size() };
    for (SourceIndex sourceIndex{}; sourceIndex < numberOfSources; ++sourceIndex) {
        auto const xName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 0) };
        auto const yName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 1) };
        auto const zName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 2) };

        auto const position{ positionSnapshots[sourceIndex].position };
        auto const elevation{ elevationsSnapshots[sourceIndex].z };

        juce::Point<float> const mirroredPosition{ position.getX(), position.getY() * -1.0f };
        auto const normalizedElevation{ elevation / MAX_ELEVATION };

        auto const mirroredNormalizedPosition{ (mirroredPosition + juce::Point<float>{ 1.0f, 1.0f }) / 2.0f };
        auto const inversedNormalizedElevation{ 1.0f - normalizedElevation };

        result->setAttribute(xName, mirroredNormalizedPosition.getX());
        result->setAttribute(yName, mirroredNormalizedPosition.getY());
        result->setAttribute(zName, inversedNormalizedElevation);
    }

    auto const xName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 0) };
    auto const yName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 1) };
    auto const zName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 2) };

    auto const position{ mSources.getPrimarySource().getPos() };
    juce::Point<float> const mirroredPosition{
        position.getX(),
        position.getY() * -1.0f
    }; // For some legacy reason, we store a normalized value with inversed Y.
    auto const inversedNormalizedPosition{ (mirroredPosition + juce::Point<float>{ 1.0f, 1.0f }) / 2.0f };
    auto const inversedNormalizedElevation{
        1.0f - mSources.getPrimarySource().getNormalizedElevation().get()
    }; // Same this happens with elevation.

    result->setAttribute(xName, inversedNormalizedPosition.getX());
    result->setAttribute(yName, inversedNormalizedPosition.getY());
    result->setAttribute(zName, inversedNormalizedElevation);

    return result;
}

//==============================================================================
void PresetsManager::save(int const presetNumber) const
{
    auto newData{ createPresetData(presetNumber) };

    // Replace an element if the new one has the same ID as one already saved.
    auto maybe_oldData{ getPresetData(presetNumber) };
    if (maybe_oldData.has_value()) {
        mData.replaceChildElement(*maybe_oldData, newData.release());
    } else {
        mData.addChildElement(newData.release());
    }

    XmlElementDataSorter sorter("ID", true);
    mData.sortChildElements(sorter);
}

//==============================================================================
bool PresetsManager::deletePreset(int const presetNumber) const
{
    auto maybe_data{ getPresetData(presetNumber) };

    if (!maybe_data.has_value()) {
        return false;
    }

    mData.removeChildElement(*maybe_data, true);
    XmlElementDataSorter sorter("ID", true);
    mData.sortChildElements(sorter);

    return true;
}

//==============================================================================
void PresetsManager::numberOfSourcesChanged()
{
    // TODO
}

//==============================================================================
std::array<bool, NUMBER_OF_POSITION_PRESETS> PresetsManager::getSavedPresets() const
{
    std::array<bool, NUMBER_OF_POSITION_PRESETS> result{};

    std::fill(std::begin(result), std::end(result), false);

    for (auto * presetData : mData.getChildIterator()) {
        auto const presetNumber{ presetData->getIntAttribute("ID") };
        result[static_cast<size_t>(presetNumber) - 1u] = true;
    }

    return result;
}

} // namespace gris
