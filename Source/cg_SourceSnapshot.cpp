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

#include "cg_SourceSnapshot.hpp"

#include "cg_Source.hpp"

namespace gris
{
SourceSnapshot::SourceSnapshot(Source const & source) noexcept : position(source.getPos()), z(source.getElevation())
{
}

SourceSnapshot const & SourcesSnapshots::operator[](SourceIndex const index) const
{
    jassert(index.get() >= 0 && index.get() < secondaries.size() + 1);
    if (index.get() == 0) {
        return primary;
    }
    return secondaries.getReference(index.get() - 1);
}

SourceSnapshot & SourcesSnapshots::operator[](SourceIndex const index)
{
    jassert(index.get() >= 0 && index.get() < secondaries.size() + 1);
    if (index.get() == 0) {
        return primary;
    }
    return secondaries.getReference(index.get() - 1);
}

} // namespace gris
