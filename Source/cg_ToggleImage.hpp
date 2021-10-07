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

#pragma once

#include <JuceHeader.h>

namespace gris
{
//==============================================================================
class ToggleImage final : public juce::TextButton
{
    juce::Image & mOffImage;
    juce::Image & mOnImage;
    juce::BorderSize<float> const & mPadding;

public:
    //==============================================================================
    ToggleImage(juce::Image & offImage, juce::Image & onImage, juce::BorderSize<float> const & padding);
    ToggleImage() = delete;
    ~ToggleImage() override = default;
    //==============================================================================
    ToggleImage(ToggleImage const &) = delete;
    ToggleImage(ToggleImage &&) = delete;
    ToggleImage & operator=(ToggleImage const &) = delete;
    ToggleImage & operator=(ToggleImage &&) = delete;
    //==============================================================================
    void paint(juce::Graphics & g) override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ToggleImage)
};

} // namespace gris