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

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class GrisLookAndFeel final : public LookAndFeel_V3
{
private:
    float mFontSize;

    Font mFont = Font(
        juce::CustomTypeface::createSystemTypefaceFor(BinaryData::SinkinSans400Regular_otf,
                                                      static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)));
    Font mBigFont = Font(
        juce::CustomTypeface::createSystemTypefaceFor(BinaryData::SinkinSans400Regular_otf,
                                                      static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)));
    Font mBiggerFont = Font(
        juce::CustomTypeface::createSystemTypefaceFor(BinaryData::SinkinSans400Regular_otf,
                                                      static_cast<size_t>(BinaryData::SinkinSans400Regular_otfSize)));

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
    GrisLookAndFeel()
    {
        mBackgroundAndFieldColor = Colour::fromRGB(75, 75, 75); // Colours::darkgrey;
        mWinBackgroundAndFieldColor = Colour::fromRGB(46, 46, 46);
        mLightColor = Colour::fromRGB(235, 245, 250); // Colours::whitesmoke;
        mDarkColor = Colour::fromRGB(15, 10, 5);      // Colours::black;
        mGreyColor = Colour::fromRGB(120, 120, 120);  // Colours::grey;
        mDisableBackgroundColor = Colour::fromRGB(150, 150, 150);
        mEditBackgroundColor = Colour::fromRGB(172, 172, 172);
        mHlBackgroundColor = Colour::fromRGB(190, 125, 18);
        mOnColor = Colour::fromRGB(255, 165, 25);
        mOnColorOver = Colour::fromRGB(255, 184, 75);
        mOnColorDown = Colour::fromRGB(222, 144, 22);
        mOffColor = Colour::fromRGB(56, 56, 56);
        mGreenColor = Colour::fromRGB(56, 156, 56);
        mRedColor = Colour::fromRGB(220, 48, 35);

        setColour(PopupMenu::highlightedBackgroundColourId, this->mOnColor);
        setColour(TextEditor::textColourId, this->mLightColor);
        setColour(TextEditor::backgroundColourId, this->mEditBackgroundColor);
        setColour(TextEditor::highlightColourId, this->mHlBackgroundColor);
        setColour(TextEditor::shadowColourId, this->mEditBackgroundColor);
        setColour(TextButton::buttonColourId, this->mEditBackgroundColor);
        setColour(ComboBox::backgroundColourId, this->mEditBackgroundColor);
        setColour(ComboBox::outlineColourId, this->mEditBackgroundColor);
        setColour(Slider::thumbColourId, this->mLightColor);
        setColour(Slider::rotarySliderFillColourId, this->mOnColor);
        setColour(Slider::trackColourId, this->mDarkColor);
        setColour(Slider::textBoxBackgroundColourId, this->mEditBackgroundColor);
        setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        setColour(TooltipWindow::ColourIds::backgroundColourId, this->mBackgroundAndFieldColor.withBrightness(0.8f));
        setColour(TooltipWindow::ColourIds::outlineColourId, this->mBackgroundAndFieldColor.withBrightness(0.8f));
        setColour(AlertWindow::backgroundColourId, this->mWinBackgroundAndFieldColor);
        setColour(AlertWindow::outlineColourId, this->mOnColor);
        setColour(AlertWindow::textColourId, this->mLightColor);
        setColour(ToggleButton::textColourId, this->mLightColor);
        setColour(Label::textColourId, this->mLightColor);

#if WIN32
        mFontSize = 13.0f;
#else
        mFontSize = 10.0f;
#endif
        mFont.setHeight(mFontSize);
        mBigFont.setHeight(mFontSize + 3.0f);
        mBiggerFont.setHeight(mFontSize + 6.0f);
    }

    [[nodiscard]] Font getFont() const { return this->mFont; }
    Font getLabelFont([[maybe_unused]] Label & label) final { return this->mFont; }
    Font getComboBoxFont([[maybe_unused]] ComboBox & comboBox) final { return this->mFont; }
    Font getTextButtonFont(TextButton &, [[maybe_unused]] int buttonHeight) final { return this->mFont; }
    Font getMenuBarFont(MenuBarComponent &,
                        [[maybe_unused]] int itemIndex,
                        [[maybe_unused]] String const & itemText) final
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
                      const bool /*isMouseButtonDown*/,
                      int buttonX,
                      int buttonY,
                      int buttonW,
                      int buttonH,
                      ComboBox & box) final
    {
        box.setColour(ColourSelector::backgroundColourId, mOnColor);

        g.fillAll(this->mEditBackgroundColor); // box.findColour (ComboBox::backgroundColourId))

        auto const arrowX{ 0.3f };
        auto const arrowH{ 0.2f };

        Path p;
        p.addTriangle(buttonX + buttonW * 0.5f,
                      buttonY + buttonH * (0.45f - arrowH),
                      buttonX + buttonW * (1.0f - arrowX),
                      buttonY + buttonH * 0.45f,
                      buttonX + buttonW * arrowX,
                      buttonY + buttonH * 0.45f);

        p.addTriangle(buttonX + buttonW * 0.5f,
                      buttonY + buttonH * (0.55f + arrowH),
                      buttonX + buttonW * (1.0f - arrowX),
                      buttonY + buttonH * 0.55f,
                      buttonX + buttonW * arrowX,
                      buttonY + buttonH * 0.55f);

        g.setColour(this->mDarkColor.withMultipliedAlpha(
            box.isEnabled() ? 1.0f : 0.3f)); // box.findColour (ComboBox::arrowColourId)
        g.fillPath(p);
    }

    void drawRoundThumb(Graphics & g,
                        float const x,
                        float const y,
                        float const diameter,
                        const Colour & color,
                        float const outlineThickness) const
    {
        const juce::Rectangle<float> a(x, y, diameter, diameter);
        auto const halfThickness{ outlineThickness * 0.5f };

        Path p;
        p.addEllipse(x + halfThickness, y + halfThickness, diameter - outlineThickness, diameter - outlineThickness);

        const DropShadow ds(this->mDarkColor, 1, Point<int>(0, 0));
        ds.drawForPath(g, p);

        g.setColour(color);
        g.fillPath(p);

        g.setColour(color.brighter());
        g.strokePath(p, PathStrokeType(outlineThickness));
    }

    void drawPopupMenuItem(Graphics & g,
                           const Rectangle<int> & area,
                           const bool isSeparator,
                           const bool isActive,
                           const bool isHighlighted,
                           const bool isTicked,
                           const bool hasSubMenu,
                           const String & text,
                           const String & shortcutKeyText,
                           const Drawable * icon,
                           const Colour * const textColourToUse) final
    {
        if (isSeparator) {
            auto r = area.reduced(5, 0);
            r.removeFromTop(r.getHeight() / 2 - 1);

            g.setColour(Colour(0x33000000));
            g.fillRect(r.removeFromTop(1));

            g.setColour(Colour(0x66ffffff));
            g.fillRect(r.removeFromTop(1));
        } else {
            auto textColor = findColour(PopupMenu::textColourId);

            if (textColourToUse != nullptr)
                textColor = *textColourToUse;

            auto r = area.reduced(1);

            if (isHighlighted) {
                g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
                g.fillRect(r);

                g.setColour(findColour(PopupMenu::highlightedTextColourId));
            } else {
                g.setColour(textColor);
            }

            if (!isActive)
                g.setOpacity(0.3f);

            auto font{ getPopupMenuFont() };

            auto const maxFontHeight{ area.getHeight() / 1.3f };

            if (font.getHeight() > maxFontHeight)
                font.setHeight(maxFontHeight);

            g.setFont(font);

            auto const iconArea{ r.removeFromLeft((r.getHeight() * 5) / 4).reduced(3).toFloat() };

            if (icon != nullptr) {
                icon->drawWithin(g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            } else if (isTicked) {
                // Removed check mark. -belangeo
                // auto tick = getTickShape (1.0f);
                // g.fillPath (tick, tick.getTransformToScaleToFit (iconArea, true));
            }

            if (hasSubMenu) {
                auto const arrowH{ 0.6f * getPopupMenuFont().getAscent() };

                auto const x{ static_cast<float>(r.removeFromRight((int)arrowH).getX()) };
                auto const halfH{ static_cast<float>(r.getCentreY()) };

                Path p;
                p.addTriangle(x, halfH - arrowH * 0.5f, x, halfH + arrowH * 0.5f, x + arrowH * 0.6f, halfH);
                g.fillPath(p);
            }

            r.removeFromRight(3);
            g.drawFittedText(text, r, Justification::centredLeft, 1);

            if (shortcutKeyText.isNotEmpty()) {
                auto f2{ font };
                f2.setHeight(f2.getHeight() * 0.75f);
                f2.setHorizontalScale(0.95f);
                g.setFont(f2);

                g.drawText(shortcutKeyText, r, Justification::centredRight, true);
            }
        }
    }

    void drawButtonBackground(Graphics & g,
                              Button & button,
                              [[maybe_unused]] Colour const & backgroundColor,
                              bool const isMouseOverButton,
                              bool const isButtonDown) final
    {
        auto const width{ button.getWidth() - 1.0f };
        auto const height{ button.getHeight() - 1.0f };
        auto const cornerSize{ jmin(15.0f, jmin(width, height) * 0.45f) };
        auto const lineThickness{ cornerSize * 0.1f };
        auto const halfThickness{ lineThickness * 0.5f };

        Path outline;
        Colour color;

        outline.addRectangle(0.5f + halfThickness, 0.5f + halfThickness, width - lineThickness, height - lineThickness);

        if (isButtonDown) {
            color = this->mOnColorDown;
        } else if (button.getToggleState()) {
            color = this->mOnColor;
        } else {
            color = button.findColour(TextButton::buttonColourId);
        }

        if (isMouseOverButton) {
            color = color.withAlpha(0.8f);
        }
        g.setColour(color);
        g.fillPath(outline);
    }

    void drawTickBox(Graphics & g,
                     Component & component,
                     float const x,
                     float const y,
                     float const w,
                     float const h,
                     bool const ticked,
                     [[maybe_unused]] bool isEnabled,
                     [[maybe_unused]] bool isMouseOverButton,
                     [[maybe_unused]] bool isButtonDown) final
    {
        auto const boxSize{ w * 0.8f };
        Rectangle<float> const r{ x, y + (h - boxSize) * 0.5f, boxSize, boxSize };

        if (ticked) {
            auto color{ mOnColor };

            if (component.isMouseOver()) {
                color = mOnColorOver;
            }

            if (!component.isEnabled()) {
                color = mOnColor.withBrightness(0.3f);
            }
            g.setColour(color);
            g.fillRect(r);

        } else {
            auto color{ mOffColor };
            if (!component.isEnabled()) {
                color = mOffColor.withBrightness(0.3f);
            }
            g.setColour(color);
            g.fillRect(r);
        }

        if (component.isEnabled() && component.isMouseButtonDown()) {
            g.setColour(this->mOnColorDown);
            g.fillRect(r);
        }
    }

    void drawLinearSliderThumb(Graphics & g,
                               int const x,
                               int const y,
                               int const width,
                               int const height,
                               float const sliderPos,
                               [[maybe_unused]] float minSliderPos,
                               [[maybe_unused]] float maxSliderPos,
                               Slider::SliderStyle const style,
                               Slider & slider) final
    {
        auto const sliderRadius{ static_cast<float>(getSliderThumbRadius(slider) - 1) };
        float kx;
        float ky;

        if (style == Slider::LinearVertical) {
            kx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
            ky = sliderPos;
        } else {
            kx = sliderPos;
            ky = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
        }
        const Rectangle<float> r(kx - (sliderRadius / 2.0f), ky - sliderRadius, 6, static_cast<float>(height) * 2.0f);

        if (slider.isEnabled()) {
            auto color{ mOnColor };

            if (slider.isMouseOver()) {
                color = mOnColorOver;
            }
            if (slider.isMouseButtonDown()) {
                color = mOnColorDown;
            }
            g.setColour(color);
            g.fillRect(r);
        } else {
            g.setColour(mOffColor);
            g.fillRect(r);
        }
    }

    void drawLinearSlider(Graphics & g,
                          int const x,
                          int const y,
                          int const width,
                          int const height,
                          float const sliderPos,
                          float const minSliderPos,
                          float const maxSliderPos,
                          Slider::SliderStyle const style,
                          Slider & slider) final
    {
        drawLinearSliderBackground(g, x, y, width, height + 2, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }

    void drawLinearSliderBackground(Graphics & g,
                                    int const x,
                                    int const y,
                                    int const width,
                                    int const height,
                                    float /*sliderPos*/,
                                    float /*minSliderPos*/,
                                    float /*maxSliderPos*/,
                                    const Slider::SliderStyle /*style*/,
                                    Slider & slider) final
    {
        auto const xFloat{ static_cast<float>(x) };
        auto const yFloat{ static_cast<float>(y) };
        auto const widthFloat{ static_cast<float>(width) };
        auto const heightFloat{ static_cast<float>(height) };

        auto const sliderRadius{ getSliderThumbRadius(slider) - 5.0f };
        Path on;
        Path off;

        if (slider.isHorizontal()) {
            auto const iy{ yFloat + heightFloat * 0.5f - sliderRadius * 0.5f };
            juce::Rectangle<float> r{ xFloat - sliderRadius * 0.5f, iy, widthFloat + sliderRadius, sliderRadius };
            auto const onW{ r.getWidth() * static_cast<float>(slider.valueToProportionOfLength(slider.getValue())) };
            on.addRectangle(r.removeFromLeft(onW));
            off.addRectangle(r);
        } else {
            auto const ix{ xFloat + widthFloat * 0.5f - sliderRadius * 0.5f };
            juce::Rectangle<float> r(ix, y - sliderRadius * 0.5f, sliderRadius, height + sliderRadius);
            auto const onH{ r.getHeight() * static_cast<float>(slider.valueToProportionOfLength(slider.getValue())) };
            on.addRectangle(r.removeFromBottom(onH));
            off.addRectangle(r);
        }

        if (slider.isEnabled()) {
            g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
            g.fillPath(on);
            g.setColour(slider.findColour(Slider::trackColourId));
            g.fillPath(off);
        } else {
            g.setColour(mOffColor);
            g.fillPath(on);
            g.fillPath(off);
        }
    }

    void fillTextEditorBackground(Graphics & g,
                                  [[maybe_unused]] int width,
                                  [[maybe_unused]] int height,
                                  TextEditor & t) final
    {
        if (t.isEnabled())
            g.setColour(this->mEditBackgroundColor);
        else
            g.setColour(this->mDisableBackgroundColor);
        g.fillAll();
    }

    void drawTextEditorOutline(Graphics & g, int const width, int const height, TextEditor & t) final
    {
        if (t.hasKeyboardFocus(true)) {
            g.setColour(this->mOnColor);
            g.drawRect(0, 0, width, height);
        }
    }

    void drawToggleButton(Graphics & g,
                          ToggleButton & button,
                          bool const isMouseOverButton,
                          bool const isButtonDown) final
    {
        if (button.hasKeyboardFocus(true)) {
            g.setColour(button.findColour(TextEditor::focusedOutlineColourId));
        }

        if (button.getButtonText().length() == 1) {
            drawTickBox(g,
                        button,
                        0,
                        0,
                        static_cast<float>(button.getWidth()),
                        static_cast<float>(button.getHeight()),
                        button.getToggleState(),
                        button.isEnabled(),
                        isMouseOverButton,
                        isButtonDown);
            g.setColour(button.findColour(ToggleButton::textColourId));
            g.setFont(mFont);

            if (!button.isEnabled())
                g.setOpacity(0.5f);

            g.drawFittedText(button.getButtonText(),
                             -2,
                             2,
                             button.getWidth(),
                             button.getHeight(),
                             Justification::centred,
                             10);

        } else {
            auto const fontSize{ jmin(15.0f, button.getHeight() * 0.75f) };
            auto const tickWidth{ fontSize * 1.1f };

            drawTickBox(g,
                        button,
                        4.0f,
                        (button.getHeight() - tickWidth) * 0.5f,
                        tickWidth,
                        tickWidth,
                        button.getToggleState(),
                        button.isEnabled(),
                        isMouseOverButton,
                        isButtonDown);

            g.setColour(button.findColour(ToggleButton::textColourId));
            g.setFont(this->mFont);

            if (!button.isEnabled())
                g.setOpacity(0.5f);

            auto const textX{ static_cast<int>(tickWidth + 5) };

            g.drawFittedText(button.getButtonText(),
                             textX,
                             0,
                             button.getWidth() - (textX - 5),
                             button.getHeight(),
                             Justification::centredLeft,
                             10);
        }
    }

    void drawTabButton(TabBarButton & button, Graphics & g, bool const isMouseOver, bool const isMouseDown) final
    {
        auto const activeArea{ button.getActiveArea() };
        activeArea.withHeight(18);
        auto const o{ button.getTabbedButtonBar().getOrientation() };
        auto const bkg{ button.getTabBackgroundColour() };

        if (button.getToggleState()) {
            g.setColour(bkg);
        } else {
            g.setColour(bkg.brighter(0.1f));
        }

        g.fillRect(activeArea);

        g.setColour(this->mWinBackgroundAndFieldColor);

        auto r{ activeArea };
        if (o != TabbedButtonBar::TabsAtTop)
            g.fillRect(r.removeFromBottom(1));
        if (o != TabbedButtonBar::TabsAtRight)
            g.fillRect(r.removeFromLeft(1));
        if (o != TabbedButtonBar::TabsAtLeft)
            g.fillRect(r.removeFromRight(1));

        auto const alpha{ button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f };
        auto const col{ bkg.contrasting().withMultipliedAlpha(alpha) };
        auto const area{ button.getTextArea().toFloat() };

        auto length{ area.getWidth() };
        auto depth{ area.getHeight() };

        if (button.getTabbedButtonBar().isVertical())
            std::swap(length, depth);

        TextLayout textLayout;
        createTabTextLayout(button, length, depth, col, textLayout);
        textLayout.draw(g, Rectangle<float>(length, depth));
    }

    void createTabTextLayout(const TabBarButton & button,
                             float const length,
                             float const depth,
                             Colour const color,
                             TextLayout & textLayout) const
    {
        auto font{ this->mFont };
#if WIN32
        font.setHeight(depth * 0.60f);
#else

        font.setHeight(depth * 0.35f);
#endif
        font.setUnderline(button.hasKeyboardFocus(false));

        AttributedString s;
        s.setJustification(Justification::centred);
        s.append(button.getButtonText().trim(), font, color);

        textLayout.createLayout(s, length);
    }

    // we don't use those, so far
    void drawRotarySlider(Graphics & g,
                          int const x,
                          int const y,
                          int const width,
                          int const height,
                          float const sliderPos,
                          float const rotaryStartAngle,
                          float const rotaryEndAngle,
                          Slider & slider) final
    {
        auto const xFloat{ static_cast<float>(x) };
        auto const yFloat{ static_cast<float>(y) };
        auto const widthFloat{ static_cast<float>(width) };
        auto const heightFloat{ static_cast<float>(height) };

        auto const radius = jmin(widthFloat / 2.0f, heightFloat / 2.0f) - 2.0f;
        auto const centreX = xFloat + widthFloat * 0.5f;
        auto const centreY = (yFloat + heightFloat * 0.5f) + 6.0f;
        auto const rx = centreX - radius;
        auto const ry = centreY - radius;
        auto const rw = radius * 2.0f;
        auto const angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto const isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        if (slider.isEnabled()) {
            if (isMouseOver) {
                g.setColour(this->mOnColorOver);
            } else {
                g.setColour(this->mOnColor);
            }
        } else {
            g.setColour(this->mOffColor);
        }
        Path filledArc;
        filledArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.0);
        g.fillPath(filledArc);
        auto const lineThickness = jmin(15.0f, jmin(widthFloat, heightFloat) * 0.45f) * 0.1f;
        Path outlineArc;
        outlineArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, 0.0);
        g.strokePath(outlineArc, PathStrokeType(lineThickness));
    }
};
