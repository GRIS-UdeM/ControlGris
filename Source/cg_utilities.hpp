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

#include <JuceHeader.h>

//==============================================================================
class XmlElementDataSorter
{
private:
    //==============================================================================
    juce::String mAttributeToSort{};
    int mDirection{};

public:
    //==============================================================================
    XmlElementDataSorter() = delete;
    ~XmlElementDataSorter() noexcept = default;

    XmlElementDataSorter(XmlElementDataSorter const &) = delete;
    XmlElementDataSorter(XmlElementDataSorter &&) = delete;

    XmlElementDataSorter & operator=(XmlElementDataSorter const &) = delete;
    XmlElementDataSorter & operator=(XmlElementDataSorter &&) = delete;
    //==============================================================================
    XmlElementDataSorter(juce::String const & attributeToSortBy, bool forwards) noexcept
        : mAttributeToSort(attributeToSortBy)
        , mDirection(forwards ? 1 : -1)
    {
    }
    //==============================================================================
    int compareElements(juce::XmlElement * first, juce::XmlElement * second) const
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
