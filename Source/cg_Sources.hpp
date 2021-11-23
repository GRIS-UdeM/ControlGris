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

#include "cg_Source.hpp"

namespace gris
{
//==============================================================================
/** A collection of sources that can be easily iterated upon. */
class Sources
{
    //==============================================================================
    struct Iterator {
        Sources * sources;
        int index;
        //==============================================================================
        bool operator!=(Iterator const & other) const { return index != other.index; }
        Iterator & operator++();
        Source & operator*() { return sources->get(index); }
        Source const & operator*() const { return sources->get(index); }
    };
    //==============================================================================
    struct ConstIterator {
        Sources const * sources;
        int index;
        //==============================================================================
        bool operator!=(ConstIterator const & other) const { return index != other.index; }
        ConstIterator & operator++();
        Source const & operator*() const { return sources->get(index); }
    };
    //==============================================================================
    int mSize{ 2 };
    Source mPrimarySource;
    std::array<Source, MAX_NUMBER_OF_SOURCES - 1> mSecondarySources{};

public:
    //==============================================================================
    void init(ControlGrisAudioProcessor * processor);
    [[nodiscard]] int size() const { return mSize; }
    void setSize(int size);
    //==============================================================================
    [[nodiscard]] Source & get(int index);
    [[nodiscard]] Source const & get(int index) const;
    [[nodiscard]] Source & get(SourceIndex const index) { return get(index.get()); }
    [[nodiscard]] Source const & get(SourceIndex const index) const { return get(index.get()); }
    [[nodiscard]] Source & operator[](int index);
    [[nodiscard]] Source const & operator[](int index) const;
    [[nodiscard]] Source & operator[](SourceIndex const index) { return (*this)[index.get()]; }
    [[nodiscard]] Source const & operator[](SourceIndex const index) const { return (*this)[index.get()]; }
    //==============================================================================
    [[nodiscard]] Source & getPrimarySource() { return mPrimarySource; }
    [[nodiscard]] Source const & getPrimarySource() const { return mPrimarySource; }
    [[nodiscard]] auto & getSecondarySources() { return mSecondarySources; }
    [[nodiscard]] auto const & getSecondarySources() const { return mSecondarySources; }
    //==============================================================================
    [[nodiscard]] Iterator begin() { return Iterator{ this, 0 }; }
    [[nodiscard]] Iterator end() { return Iterator{ this, mSize }; }
    [[nodiscard]] ConstIterator begin() const { return ConstIterator{ this, 0 }; }
    [[nodiscard]] ConstIterator end() const { return ConstIterator{ this, mSize }; }
    [[nodiscard]] ConstIterator cbegin() const { return ConstIterator{ this, 0 }; }
    [[nodiscard]] ConstIterator cend() const { return ConstIterator{ this, mSize }; }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(Sources)
};
} // namespace gris