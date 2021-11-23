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

#include "cg_Sources.hpp"

namespace gris
{
Sources::Iterator & Sources::Iterator::operator++()
{
    ++index;
    return *this;
}

Sources::ConstIterator & Sources::ConstIterator::operator++()
{
    ++index;
    return *this;
}

//==============================================================================
void Sources::setSize(int const size)
{
    jassert(size >= 1 && size <= MAX_NUMBER_OF_SOURCES);
    mSize = size;
    auto const azimuthSpan{ mPrimarySource.getAzimuthSpan() };
    auto const elevationSpan{ mPrimarySource.getElevationSpan() };
    for (auto & source : *this) {
        source.setColorFromIndex(size);
        source.setAzimuthSpan(azimuthSpan);
        source.setElevationSpan(elevationSpan);
    }
}

Source & Sources::get(int const index)
{
    jassert(index >= 0 && index < MAX_NUMBER_OF_SOURCES); // TODO: should check for mSize
    if (index == 0) {
        return mPrimarySource;
    }
    return mSecondarySources[static_cast<size_t>(index) - 1u];
}

Source const & Sources::get(int const index) const
{
    jassert(index >= 0 && index < MAX_NUMBER_OF_SOURCES); // TODO: should check for mSize
    if (index == 0) {
        return mPrimarySource;
    }
    return mSecondarySources[static_cast<size_t>(index) - 1u];
}

Source & Sources::operator[](int const index)
{
    jassert(index >= 0 && index < MAX_NUMBER_OF_SOURCES); // TODO: should check for mSize
    if (index == 0) {
        return mPrimarySource;
    }
    return mSecondarySources[static_cast<size_t>(index) - 1u];
}

Source const & Sources::operator[](int const index) const
{
    jassert(index >= 0 && index < MAX_NUMBER_OF_SOURCES); // TODO: should check for mSize
    if (index == 0) {
        return mPrimarySource;
    }
    return mSecondarySources[static_cast<size_t>(index) - 1u];
}

void Sources::init(ControlGrisAudioProcessor * processor)
{
    SourceIndex currentIndex{};
    mPrimarySource.setIndex(currentIndex++);
    mPrimarySource.setProcessor(processor);
    for (auto & secondarySource : mSecondarySources) {
        secondarySource.setIndex(currentIndex++);
        secondarySource.setProcessor(processor);
    }
}
} // namespace gris
