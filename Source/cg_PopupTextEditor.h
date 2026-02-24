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

#pragma once

#include <JuceHeader.h>

namespace gris
{
//==============================================================================
class PopupTextEditor
: public juce::PopupMenu::CustomComponent
, public juce::Timer
{
public:
    enum class popupMenuItems{ copy = 100, paste };
    //==============================================================================
    class InnerTextEditor : public juce::TextEditor
    {
    public:
        explicit InnerTextEditor(const juce::String& name) : juce::TextEditor (name) {}
        InnerTextEditor() = delete;
        ~InnerTextEditor() override = default;
        //==============================================================================
        void addPopupMenuItems(juce::PopupMenu & menuToAddTo, const juce::MouseEvent * mouseClickEvent) override;
        void performPopupMenuAction(int menuItemID) override;
    private:
        JUCE_LEAK_DETECTOR(InnerTextEditor)
    };
    //==============================================================================
    PopupTextEditor();
    ~PopupTextEditor() override;
    
    //==============================================================================
    void getIdealSize(int &idealWidth, int &idealHeight) override;
    void timerCallback() override;
    
    void setPopupTextEditor(std::unique_ptr<InnerTextEditor> textEditor);
    
private:
    //==============================================================================
    std::unique_ptr<InnerTextEditor> mPopupTextEditor;
    
    //==============================================================================
    JUCE_LEAK_DETECTOR(PopupTextEditor)
};

} // namespace gris
