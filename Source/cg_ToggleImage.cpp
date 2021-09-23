/*
 This file is part of SpatGRIS.

 Developers: Samuel Béland, Olivier Bélanger, Nicolas Masson

 SpatGRIS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SpatGRIS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SpatGRIS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cg_ToggleImage.hpp"

namespace gris
{
//==============================================================================
ToggleImage::ToggleImage(juce::Image & offImage, juce::Image & onImage, juce::BorderSize<float> const & padding)
    : mOffImage(offImage)
    , mOnImage(onImage)
    , mPadding(padding)
{
    setClickingTogglesState(true);
}

//==============================================================================
void ToggleImage::paint(juce::Graphics & g)
{
    TextButton::paint(g);

    auto const getImageBounds = [&]() {
        auto const initialBounds{ getLocalBounds().toFloat() };
        return initialBounds.withTrimmedTop(mPadding.getTop())
            .withTrimmedRight(mPadding.getRight())
            .withTrimmedBottom(mPadding.getBottom())
            .withTrimmedLeft(mPadding.getLeft());
    };
    auto const getImageToDraw = [&]() -> juce::Image const & {
        if (getToggleState() || isMouseOver(true)) {
            return mOnImage;
        }
        return mOffImage;
    };

    g.drawImage(getImageToDraw(), getImageBounds());
}
} // namespace gris
