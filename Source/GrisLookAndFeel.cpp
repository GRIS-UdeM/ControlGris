#include "GrisLookAndFeel.h"

//==============================================================================
GrisLookAndFeel::GrisLookAndFeel()
{
    mBackgroundAndFieldColor = Colour::fromRGB(75, 75, 75);
    mWinBackgroundAndFieldColor = Colour::fromRGB(46, 46, 46);
    mLightColor = Colour::fromRGB(235, 245, 250);
    mDarkColor = Colour::fromRGB(15, 10, 5);
    mGreyColor = Colour::fromRGB(120, 120, 120);
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

//==============================================================================
void GrisLookAndFeel::drawComboBox(Graphics & g,
                                   int,
                                   int,
                                   const bool,
                                   int const buttonX,
                                   int const buttonY,
                                   int const buttonW,
                                   int const /*buttonH*/,
                                   ComboBox & box)
{
    box.setColour(ColourSelector::backgroundColourId, mOnColor);

    g.fillAll(this->mEditBackgroundColor); // box.findColour (ComboBox::backgroundColourId))

    auto const arrowX{ 0.3f };
    auto const arrowH{ 0.2f };

    auto const buttonXFloat{ static_cast<float>(buttonX) };
    auto const buttonYFloat{ static_cast<float>(buttonY) };
    auto const buttonWFloat{ static_cast<float>(buttonW) };
    auto const buttonHFloat{ static_cast<float>(buttonX) };

    Path p;
    p.addTriangle(buttonXFloat + buttonWFloat * 0.5f,
                  buttonYFloat + buttonHFloat * (0.45f - arrowH),
                  buttonXFloat + buttonWFloat * (1.0f - arrowX),
                  buttonYFloat + buttonHFloat * 0.45f,
                  buttonXFloat + buttonWFloat * arrowX,
                  buttonYFloat + buttonHFloat * 0.45f);

    p.addTriangle(buttonXFloat + buttonWFloat * 0.5f,
                  buttonYFloat + buttonHFloat * (0.55f + arrowH),
                  buttonXFloat + buttonWFloat * (1.0f - arrowX),
                  buttonYFloat + buttonHFloat * 0.55f,
                  buttonXFloat + buttonWFloat * arrowX,
                  buttonYFloat + buttonHFloat * 0.55f);

    g.setColour(this->mDarkColor.withMultipliedAlpha(
        box.isEnabled() ? 1.0f : 0.3f)); // box.findColour (ComboBox::arrowColourId)
    g.fillPath(p);
}

//==============================================================================
void GrisLookAndFeel::drawRoundThumb(Graphics & g,
                                     float const x,
                                     float const y,
                                     float const diameter,
                                     const Colour & color,
                                     float const outlineThickness) const
{
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

//==============================================================================
void GrisLookAndFeel::drawPopupMenuItem(Graphics & g,
                                        const Rectangle<int> & area,
                                        const bool isSeparator,
                                        const bool isActive,
                                        const bool isHighlighted,
                                        const bool isTicked,
                                        const bool hasSubMenu,
                                        const String & text,
                                        const String & shortcutKeyText,
                                        const Drawable * icon,
                                        const Colour * const textColourToUse)
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

            auto const x{ static_cast<float>(r.removeFromRight(static_cast<int>(arrowH)).getX()) };
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

//==============================================================================
void GrisLookAndFeel::drawButtonBackground(Graphics & g,
                                           Button & button,
                                           Colour const & /*backgroundColor*/,
                                           bool const isMouseOverButton,
                                           bool const isButtonDown)
{
    auto const width{ static_cast<float>(button.getWidth()) - 1.0f };
    auto const height{ static_cast<float>(button.getHeight()) - 1.0f };
    auto const cornerSize{ std::min(15.0f, std::min(width, height) * 0.45f) };
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

//==============================================================================
void GrisLookAndFeel::drawTickBox(Graphics & g,
                                  Component & component,
                                  float const x,
                                  float const y,
                                  float const w,
                                  float const h,
                                  bool const ticked,
                                  bool /*isEnabled*/,
                                  bool /*isMouseOverButton*/,
                                  bool /*isButtonDown*/)
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

//==============================================================================
void GrisLookAndFeel::drawLinearSliderThumb(Graphics & g,
                                            int const x,
                                            int const y,
                                            int const width,
                                            int const height,
                                            float const sliderPos,
                                            float /*minSliderPos*/,
                                            float /*maxSliderPos*/,
                                            Slider::SliderStyle const style,
                                            Slider & slider)
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

//==============================================================================
void GrisLookAndFeel::drawLinearSlider(Graphics & g,
                                       int const x,
                                       int const y,
                                       int const width,
                                       int const height,
                                       float const sliderPos,
                                       float const minSliderPos,
                                       float const maxSliderPos,
                                       Slider::SliderStyle const style,
                                       Slider & slider)
{
    drawLinearSliderBackground(g, x, y, width, height + 2, sliderPos, minSliderPos, maxSliderPos, style, slider);
    drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

//==============================================================================
void GrisLookAndFeel::drawLinearSliderBackground(Graphics & g,
                                                 int const x,
                                                 int const y,
                                                 int const width,
                                                 int const height,
                                                 float,
                                                 float,
                                                 float,
                                                 const Slider::SliderStyle,
                                                 Slider & slider)
{
    auto const xFloat{ static_cast<float>(x) };
    auto const yFloat{ static_cast<float>(y) };
    auto const widthFloat{ static_cast<float>(width) };
    auto const heightFloat{ static_cast<float>(height) };

    auto const sliderRadius{ static_cast<float>(getSliderThumbRadius(slider)) - 5.0f };
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
        juce::Rectangle<float> r{ ix,
                                  static_cast<float>(y) - sliderRadius * 0.5f,
                                  sliderRadius,
                                  static_cast<float>(height) + sliderRadius };
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

//==============================================================================
void GrisLookAndFeel::fillTextEditorBackground(Graphics & g, int /*width*/, int /*height*/, TextEditor & t)
{
    if (t.isEnabled())
        g.setColour(this->mEditBackgroundColor);
    else
        g.setColour(this->mDisableBackgroundColor);
    g.fillAll();
}

//==============================================================================
void GrisLookAndFeel::drawTextEditorOutline(Graphics & g, int const width, int const height, TextEditor & t)
{
    if (t.hasKeyboardFocus(true)) {
        g.setColour(this->mOnColor);
        g.drawRect(0, 0, width, height);
    }
}

//==============================================================================
void GrisLookAndFeel::drawToggleButton(Graphics & g,
                                       ToggleButton & button,
                                       bool const isMouseOverButton,
                                       bool const isButtonDown)
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
        auto const fontSize{ std::min(15.0f, static_cast<float>(button.getHeight()) * 0.75f) };
        auto const tickWidth{ fontSize * 1.1f };

        drawTickBox(g,
                    button,
                    4.0f,
                    (static_cast<float>(button.getHeight()) - tickWidth) * 0.5f,
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

//==============================================================================
void GrisLookAndFeel::drawTabButton(TabBarButton & button, Graphics & g, bool const isMouseOver, bool const isMouseDown)
{
    auto const activeArea{ button.getActiveArea() };
    // activeArea.withHeight(18);
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

//==============================================================================
void GrisLookAndFeel::createTabTextLayout(const TabBarButton & button,
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

//==============================================================================
void GrisLookAndFeel::drawRotarySlider(Graphics & g,
                                       int const x,
                                       int const y,
                                       int const width,
                                       int const height,
                                       float const sliderPos,
                                       float const rotaryStartAngle,
                                       float const rotaryEndAngle,
                                       Slider & slider)
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
