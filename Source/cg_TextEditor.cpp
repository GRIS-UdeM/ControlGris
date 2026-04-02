/**************************************************************************
 * Copyright 2025 UdeM - GRIS - Gaël LANE LÉPINE                          *
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
 * <https://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#include "cg_TextEditor.hpp"
#include "cg_PopupTextEditor.h"

namespace gris
{
//==============================================================================
TextEd::TextEd(GrisLookAndFeel & glaf) : mGrisLookAndFeel(glaf)
{
    setReadOnly(true);
    setCaretVisible(false);

    mCopyPasteMenu.setLookAndFeel(&mGrisLookAndFeel);
    mCopyPasteMenu.addItem(static_cast<int>(popupMenuItems::copy), "Copy");
    mCopyPasteMenu.addItem(static_cast<int>(popupMenuItems::paste), "Paste");
}

//==============================================================================
TextEd::~TextEd()
{
    juce::PopupMenu::dismissAllActiveMenus();
    setLookAndFeel(nullptr);
}

//==============================================================================
void TextEd::mouseDown(const juce::MouseEvent & event)
{
    if (event.mods.isRightButtonDown()) {
        mCopyPasteMenu.showMenuAsync(juce::PopupMenu::Options(), [this](int result) {
            switch (result) {
                case static_cast<int>(popupMenuItems::copy):
                    juce::SystemClipboard::copyTextToClipboard(getText());
                    break;
                case static_cast<int>(popupMenuItems::paste):
                {
                    juce::TextEditor tempEd;
                    auto text{ juce::SystemClipboard::getTextFromClipboard() };
                    auto inputFilter{ getInputFilter() };
                    if (inputFilter == nullptr) {
                        return;
                    }
                    juce::String filtered{ inputFilter->filterNewText(tempEd, text) };
                    setTextFromPopupTextEditor(filtered);
                }
                    break;

                default:
                    break;
            }
        });
    }

    if (!mIsEditable)
        return;

    // Prevent simple clicks from starting edit mode or mouse selection
    if (event.getNumberOfClicks() == 1 || isReadOnly()) {
        unfocusAllComponents();
        return;
    }
}

//==============================================================================
void TextEd::mouseDoubleClick(const juce::MouseEvent & /*event*/)
{
    if (!isEnabled() || !mIsEditable)
        return;

    auto setTextFromPopupTextEditorLambda
        = [this](const juce::String & newText) { this->setTextFromPopupTextEditor(newText); };
    auto popupEditor{ std::make_unique<PopupTextEditor::InnerTextEditor>(setTextFromPopupTextEditorLambda,
                                                                         "TextEdEditor") };
    popupEditor->setJustification(juce::Justification::centred);
    popupEditor->setMultiLine(false);
    popupEditor->setSize(getWidth() + 20, 20);
    popupEditor->setInputFilter(getInputFilter(), false);
    popupEditor->setText(getText(), false);
    popupEditor->selectAll();

    auto popupTextEditor{ std::make_unique<PopupTextEditor>() };
    popupTextEditor->initPopupTextEditor(std::move(popupEditor));

    mEditorPopupMenu.clear();
    mEditorPopupMenu.addCustomItem(1, std::move(popupTextEditor));
    mEditorPopupMenu.showMenuAsync(juce::PopupMenu::Options(), [](int result) {});
}

//==============================================================================
void TextEd::setTextFromPopupTextEditor(juce::String newText)
{
    if (!newText.isEmpty()) {
        auto text = newText.replace(",", ".");
        setText(text, juce::sendNotification);
        onFocusLost();
    }
    juce::PopupMenu::dismissAllActiveMenus();
}

//==============================================================================
void TextEd::setEditable(bool isEditable)
{
    mIsEditable = isEditable;
}

} // namespace gris
