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

#include "cg_IsRelease.hpp"

#include <JuceHeader.h>

#include <optional>

namespace gris
{
template<typename T>
juce::String const & enumToString(T const & value, juce::StringArray const & strings) noexcept(IS_RELEASE)
{
    jassert(!strings.isEmpty());
    auto const index{ static_cast<int>(value) };
    if (index < 0 || index >= strings.size()) {
        jassertfalse;
        return strings.getReference(0);
    }
    return strings.getReference(index);
}

template<typename T>
std::optional<T> stringToEnum(juce::String const & string, juce::StringArray const & strings) noexcept(IS_RELEASE)
{
    static constexpr auto INVALID_INDEX = -1;

    auto const index{ strings.indexOf(string, true) };
    if (index == INVALID_INDEX) {
        return std::nullopt;
    }

    return static_cast<T>(index);
}

//==============================================================================
class XmlElementDataSorter
{
    juce::String mAttributeToSort{};
    int mDirection{};

public:
    //==============================================================================
    XmlElementDataSorter(juce::String attributeToSortBy, bool const forwards) noexcept
        : mAttributeToSort(std::move(attributeToSortBy))
        , mDirection(forwards ? 1 : -1)
    {
    }
    XmlElementDataSorter() = delete;
    ~XmlElementDataSorter() noexcept = default;
    //==============================================================================
    XmlElementDataSorter(XmlElementDataSorter const &) = delete;
    XmlElementDataSorter(XmlElementDataSorter &&) = delete;
    XmlElementDataSorter & operator=(XmlElementDataSorter const &) = delete;
    XmlElementDataSorter & operator=(XmlElementDataSorter &&) = delete;
    //==============================================================================
    [[nodiscard]] int compareElements(juce::XmlElement const * first, juce::XmlElement const * second) const
    {
        int result;
        if (first->getDoubleAttribute(mAttributeToSort) < second->getDoubleAttribute(mAttributeToSort))
            result = -1;
        else if (first->getDoubleAttribute(mAttributeToSort) > second->getDoubleAttribute(mAttributeToSort))
            result = 1;
        else
            result = 0;

        return mDirection * result;
    }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(XmlElementDataSorter)
};

} // namespace gris