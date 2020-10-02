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
class GrisLookAndFeel final : public LookAndFeel_V3
{
    float mFontSize;
    Font mFont{ juce::CustomTypeface::createSystemTypefaceFor(
        BinaryData::SinkinSans400Regular_otf,
        static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)) };
    Font mBigFont{ juce::CustomTypeface::createSystemTypefaceFor(
        BinaryData::SinkinSans400Regular_otf,
        static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)) };
    Font mBiggerFont{ juce::CustomTypeface::createSystemTypefaceFor(
        BinaryData::SinkinSans400Regular_otf,
        static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)) };

    Colour mBackgroundAndFieldColor;
    Colour mWinBackgroundAndFieldColor;
    Colour mLightColor;
    Colour mDarkColor;
    Colour mGreyColor;
    Colour mEditBackgroundColor;
    Colour mDisableBackgroundColor;
    Colour mHlBackgroundColor;
    Colour mOnColor;
    Colour mOnColorOver;
    Colour mOnColorDown;
    Colour mOffColor;
    Colour mGreenColor;
    Colour mRedColor;

public:
    //==============================================================================
    GrisLookAndFeel();

    [[nodiscard]] Font getFont() const { return this->mFont; }
    Font getLabelFont([[maybe_unused]] Label & label) override { return this->mFont; }
    Font getComboBoxFont([[maybe_unused]] ComboBox & comboBox) override { return this->mFont; }
    Font getTextButtonFont(TextButton &, [[maybe_unused]] int buttonHeight) override { return this->mFont; }
    Font getMenuBarFont(MenuBarComponent &,
                        [[maybe_unused]] int itemIndex,
                        [[maybe_unused]] String const & itemText) override
    {
        return this->mFont;
    }

    [[nodiscard]] Colour getWinBackgroundColor() const { return this->mWinBackgroundAndFieldColor; }
    [[nodiscard]] Colour getBackgroundColor() const { return this->mBackgroundAndFieldColor; }
    [[nodiscard]] Colour getFieldColor() const { return this->mBackgroundAndFieldColor; }
    [[nodiscard]] Colour getFontColor() const { return this->mLightColor; }
    [[nodiscard]] Colour getScrollBarColor() const { return this->mGreyColor; }
    [[nodiscard]] Colour getDarkColor() const { return this->mDarkColor; }
    [[nodiscard]] Colour getLightColor() const { return this->mLightColor; }
    [[nodiscard]] Colour getEditBackgroundColor() const { return this->mEditBackgroundColor; }
    [[nodiscard]] Colour getHighlightColor() const { return this->mHlBackgroundColor; }
    [[nodiscard]] Colour getOnColor() const { return this->mOnColor; }
    [[nodiscard]] Colour getOffColor() const { return this->mOffColor; }
    [[nodiscard]] Colour getGreenColor() const { return this->mGreenColor; }
    [[nodiscard]] Colour getRedColor() const { return this->mRedColor; }

    // https://github.com/audioplastic/Juce-look-and-feel-examples/blob/master/JuceLibraryCode/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.cpp

    void drawComboBox(Graphics & g,
                      int /*width*/,
                      int /*height*/,
                      bool /*isMouseButtonDown*/,
                      int buttonX,
                      int buttonY,
                      int buttonW,
                      int buttonH,
                      ComboBox & box) override;

    void drawRoundThumb(Graphics & g, float x, float y, float diameter, const Colour & color, float outlineThickness)
        const;

    void drawPopupMenuItem(Graphics & g,
                           const Rectangle<int> & area,
                           bool isSeparator,
                           bool isActive,
                           bool isHighlighted,
                           bool isTicked,
                           bool hasSubMenu,
                           const String & text,
                           const String & shortcutKeyText,
                           const Drawable * icon,
                           const Colour * textColourToUse) override;

    void drawButtonBackground(Graphics & g,
                              Button & button,
                              [[maybe_unused]] Colour const & backgroundColor,
                              bool isMouseOverButton,
                              bool isButtonDown) override;

    void drawTickBox(Graphics & g,
                     Component & component,
                     float x,
                     float y,
                     float w,
                     float h,
                     bool ticked,
                     [[maybe_unused]] bool isEnabled,
                     [[maybe_unused]] bool isMouseOverButton,
                     [[maybe_unused]] bool isButtonDown) override;

    void drawLinearSliderThumb(Graphics & g,
                               int x,
                               int y,
                               int width,
                               int height,
                               float sliderPos,
                               [[maybe_unused]] float minSliderPos,
                               [[maybe_unused]] float maxSliderPos,
                               Slider::SliderStyle const style,
                               Slider & slider) override;

    void drawLinearSlider(Graphics & g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float minSliderPos,
                          float maxSliderPos,
                          Slider::SliderStyle style,
                          Slider & slider) override;

    void drawLinearSliderBackground(Graphics & g,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    float /*sliderPos*/,
                                    float /*minSliderPos*/,
                                    float /*maxSliderPos*/,
                                    Slider::SliderStyle /*style*/,
                                    Slider & slider) override;

    void fillTextEditorBackground(Graphics & g,
                                  [[maybe_unused]] int width,
                                  [[maybe_unused]] int height,
                                  TextEditor & t) override;

    void drawTextEditorOutline(Graphics & g, int width, int height, TextEditor & t) override;

    void drawToggleButton(Graphics & g, ToggleButton & button, bool isMouseOverButton, bool isButtonDown) override;

    void drawTabButton(TabBarButton & button, Graphics & g, bool isMouseOver, bool isMouseDown) override;

    void createTabTextLayout(const TabBarButton & button,
                             float length,
                             float depth,
                             Colour color,
                             TextLayout & textLayout) const;

    // we don't use those, so far
    void drawRotarySlider(Graphics & g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          Slider & slider) override;
};
