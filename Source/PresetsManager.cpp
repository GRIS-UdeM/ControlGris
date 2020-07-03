/*
  ==============================================================================

    PresetsManager.cpp
    Created: 3 Jul 2020 2:51:56pm
    Author:  samuel

  ==============================================================================
*/

#include "PresetsManager.h"

#include "ControlGrisUtilities.h"

enum class FixedPositionType { terminal, initial };

//==============================================================================
String getFixedPosSourceName(FixedPositionType const fixedPositionType, SourceIndex const index, int const dimension)
{
    String const type{ fixedPositionType == FixedPositionType::terminal ? "_terminal" : "" };
    String result{};
    switch (dimension) {
    case 0:
        result = String("S") + String(index.toInt() + 1) + type + String("_X");
        break;
    case 1:
        result = String("S") + String(index.toInt() + 1) + type + String("_Y");
        break;
    case 2:
        result = String("S") + String(index.toInt() + 1) + type + String("_Z");
        break;
    default:
        jassertfalse; // how did you get there?
    }
    return result;
}

PresetsManager::PresetsManager(XmlElement & data,
                               Sources & sources,
                               SourceLinkEnforcer & positionLinkEnforcer,
                               SourceLinkEnforcer & elevationLinkEnforcer)
    : mData(data)
    , mSources(sources)
    , mPositionLinkEnforcer(positionLinkEnforcer)
    , mElevationLinkEnforcer(elevationLinkEnforcer)
{
    subscribeToSources();
}

PresetsManager::~PresetsManager() noexcept
{
    unsubscribeToSources();
}

bool PresetsManager::loadIfPresetChanged(int const presetNumber)
{
    if (presetNumber == mLastLoadedPreset) {
        return true;
    }

    return load(presetNumber);
}

bool PresetsManager::forceLoad(int const presetNumber)
{
    return load(presetNumber);
}

bool PresetsManager::load(int const presetNumber)
{
    if (presetNumber != 0) {
        auto const maybe_presetData{ getPresetData(presetNumber) };

        if (!maybe_presetData.has_value()) {
            return false;
        }

        auto const * presetData{ maybe_presetData.value() };

        SourcesSnapshots snapshots{};
        for (auto & source : mSources) {
            SourceSnapshot snapshot{};
            auto const index{ source.getIndex() };
            auto const xPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 0) };
            auto const yPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 1) };
            if (presetData->hasAttribute(xPosId) && presetData->hasAttribute(yPosId)) {
                Point<float> const position{ static_cast<float>(presetData->getDoubleAttribute(xPosId)),
                                             static_cast<float>(presetData->getDoubleAttribute(yPosId)) };
                snapshot.position = position;
                auto const zPosId{ getFixedPosSourceName(FixedPositionType::initial, index, 2) };
                if (presetData->hasAttribute(zPosId)) {
                    auto const normalizedElevation{ static_cast<float>(
                        presetData->getDoubleAttribute(getFixedPosSourceName(FixedPositionType::initial, index, 2))) };
                    snapshot.z = MAX_ELEVATION * normalizedElevation;
                }
            }
            if (source.isPrimarySource()) {
                snapshots.primary = snapshot;
            } else {
                snapshots.secondaries.add(snapshot);
            }
        }

        mPositionLinkEnforcer.loadSnapshots(snapshots);

        mElevationLinkEnforcer.loadSnapshots(snapshots);
        //    if (mSpatMode == SpatMode::cube) {
        //        mElevationLinkEnforcer.loadSnapshots(snapshots);
        //    }

        auto const xTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 0) };
        auto const yTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 1) };
        auto const zTerminalPositionId{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 2) };

        Point<float> terminalPosition{};
        if (presetData->hasAttribute(xTerminalPositionId) && presetData->hasAttribute(yTerminalPositionId)) {
            terminalPosition.setXY(static_cast<float>(presetData->getDoubleAttribute(xTerminalPositionId)),
                                   static_cast<float>(presetData->getDoubleAttribute(yTerminalPositionId)));
        } else {
            terminalPosition = snapshots.primary.position;
        }
        mSources.getPrimarySource().setPos(terminalPosition, SourceLinkNotification::notify);

        Radians elevation{};
        if (presetData->hasAttribute(zTerminalPositionId)) {
            elevation = MAX_ELEVATION * static_cast<float>(presetData->getDoubleAttribute(zTerminalPositionId));
        } else {
            elevation = snapshots.primary.z;
        };
        mSources.getPrimarySource().setElevation(elevation, SourceLinkNotification::notify);
        //    if (mSpatMode == SpatMode::cube) {
        //        Radians elevation{};
        //        if (presetData->hasAttribute(zTerminalPositionId)) {
        //            elevation
        //                    = MAX_ELEVATION * static_cast<float>(presetData->getDoubleAttribute(zTerminalPositionId));
        //        } else {
        //            elevation = snapshots.primary.z;
        //        };
        //        mSources.getPrimarySource().setElevation(elevation, SourceLinkNotification::notify);
        //    }

        //    // refresh trajectory
        //    mPositionAutomationManager.setTrajectoryType(mPositionAutomationManager.getTrajectoryType(),
        //                                                 mSources.getPrimarySource().getPos());
    }

    mLastLoadedPreset = presetNumber;

    return true;
}

bool PresetsManager::contains(int const presetNumber) const
{
    auto const presetData{ getPresetData(presetNumber) };
    return presetData.has_value();
}

optional<XmlElement *> PresetsManager::getPresetData(int const presetNumber) const
{
    forEachXmlChildElement(mData, element)
    {
        if (element->getIntAttribute("ID") == presetNumber) {
            return element;
        }
    }

    return nullopt;
}

std::unique_ptr<XmlElement> PresetsManager::createPresetData(int const presetNumber) const
{
    // Build a new fixed position element.
    auto result{ std::make_unique<XmlElement>("ITEM") };
    result->setAttribute("ID", presetNumber);

    auto const & snapshots{ mPositionLinkEnforcer.getSnapshots() };

    SourceIndex const numberOfSources{ snapshots.size() };
    for (SourceIndex sourceIndex{}; sourceIndex < numberOfSources; ++sourceIndex) {
        auto const xName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 0) };
        auto const yName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 1) };
        auto const zName{ getFixedPosSourceName(FixedPositionType::initial, sourceIndex, 2) };

        auto const position{ snapshots[sourceIndex].position };
        auto const zValue{ snapshots[sourceIndex].z / MAX_ELEVATION };

        result->setAttribute(xName, position.getX());
        result->setAttribute(yName, position.getY());
        result->setAttribute(zName, zValue);
    }

    auto const xName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 0) };
    auto const yName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 1) };
    auto const zName{ getFixedPosSourceName(FixedPositionType::terminal, SourceIndex{ 0 }, 2) };

    auto const position{ mSources.getPrimarySource().getPos() };
    auto const zValue{ mSources.getPrimarySource().getElevation() / MAX_ELEVATION };

    result->setAttribute(xName, position.getX());
    result->setAttribute(yName, position.getY());
    result->setAttribute(zName, zValue);

    return result;
}

void PresetsManager::save(int const presetNumber)
{
    auto newData{ createPresetData(presetNumber) };

    // Replace an element if the new one has the same ID as one already saved.
    auto maybe_oldData{ getPresetData(presetNumber) };
    if (maybe_oldData.has_value()) {
        mData.replaceChildElement(maybe_oldData.value(), newData.release());
    } else {
        mData.addChildElement(newData.release());
    }

    XmlElementDataSorter sorter("ID", true);
    mData.sortChildElements(sorter);
}

bool PresetsManager::deletePreset(int presetNumber)
{
    auto maybe_data{ getPresetData(presetNumber) };

    if (!maybe_data.has_value()) {
        return false;
    }

    mData.removeChildElement(maybe_data.value(), true);
    XmlElementDataSorter sorter("ID", true);
    mData.sortChildElements(sorter);
}

std::array<bool, NUMBER_OF_POSITION_PRESETS> PresetsManager::getSavedPresets() const
{
    std::array<bool, NUMBER_OF_POSITION_PRESETS> result{};

    std::fill(std::begin(result), std::end(result), false);

    forEachXmlChildElement(mData, presetData)
    {
        auto const presetNumber{ presetData->getIntAttribute("ID") };
        result[presetNumber - 1] = true;
    }

    return result;
}

void PresetsManager::changeListenerCallback(ChangeBroadcaster * broadcaster)
{
    mSourceMovedSinceLastRecall = true;
}

void PresetsManager::subscribeToSources()
{
    for (auto & source : mSources) {
        source.addSourceLinkListener(this);
    }
}

void PresetsManager::unsubscribeToSources()
{
    for (auto & source : mSources) {
        source.removeSourceLinkListener(this);
    }
}
