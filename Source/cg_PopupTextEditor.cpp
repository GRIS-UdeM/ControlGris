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

#include "cg_PopupTextEditor.h"

namespace gris
{
//==============================================================================
PopupTextEditor::InnerTextEditor::InnerTextEditor(std::function<void(const juce::String&)> fn, const juce::String& name)
    : juce::TextEditor (name)
    , setValFn(std::move(fn))
{
    setLookAndFeel(&mGrisLookAndFeel);
}

//==============================================================================
void PopupTextEditor::InnerTextEditor::addPopupMenuItems(juce::PopupMenu & menuToAddTo, const juce::MouseEvent * mouseClickEvent)
{
    menuToAddTo.addItem (static_cast<int>(popupMenuItems::copy), "Copy");
    menuToAddTo.addItem (static_cast<int>(popupMenuItems::paste), "Paste");
}

//==============================================================================
void PopupTextEditor::InnerTextEditor::performPopupMenuAction(int menuItemID)
{
    switch (menuItemID) {
        case static_cast<int>(popupMenuItems::copy):
            copy();
            break;
        case static_cast<int>(popupMenuItems::paste):
        {
            juce::TextEditor tempEd;
            auto text{ juce::SystemClipboard::getTextFromClipboard() };
            auto inputFilter{ getInputFilter() };
            juce::String filtered{ inputFilter->filterNewText(tempEd, text) };
            setValFn(filtered);
        }
            break;
            
        default:
            break;
    }
}

//==============================================================================
void PopupTextEditor::InnerTextEditor::returnPressed()
{
    auto newText{ getText() };
    setValFn(newText);
    juce::PopupMenu::dismissAllActiveMenus();
}

//==============================================================================
PopupTextEditor::PopupTextEditor() : juce::PopupMenu::CustomComponent(false)
{
    startTimer(100);
}

//==============================================================================
PopupTextEditor::~PopupTextEditor()
{
    stopTimer();
    mPopupTextEditor->setLookAndFeel(nullptr);
    mPopupTextEditor.reset();
}

//==============================================================================
void PopupTextEditor::getIdealSize(int &idealWidth, int &idealHeight)
{
    idealWidth = mPopupTextEditor->getWidth();
    idealHeight = mPopupTextEditor->getHeight();
}

//==============================================================================
void PopupTextEditor::initPopupTextEditor(std::unique_ptr<InnerTextEditor> textEditor)
{
    mPopupTextEditor = std::move(textEditor);
    addAndMakeVisible(*mPopupTextEditor);
}

//==============================================================================
void PopupTextEditor::timerCallback()
{
    if (isShowing()) {
        if (! hasKeyboardFocus(true)) {
            grabKeyboardFocus();
        }
    }
}
} // namespace gris
