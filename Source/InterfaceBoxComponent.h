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

#include "GrisLookAndFeel.h"

//==============================================================================
class InterfaceBoxComponent final
    : public Component
    , public TextEditor::Listener
{
public:
    struct Listener {
        virtual ~Listener() {}

        virtual void oscOutputPluginIdChanged(int value) = 0;
        virtual void oscInputConnectionChanged(bool state, int oscPort) = 0;
        virtual void oscOutputConnectionChanged(bool state, String oscAddress, int oscPort) = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    ListenerList<Listener> mListeners{};

    ToggleButton mOscReceiveToggle{};
    ToggleButton mOscSendToggle{};

    Label mOscOutputPluginIdLabel{};
    TextEditor mOscOutputPluginIdEditor{};

    TextEditor mOscReceiveIpEditor{};
    TextEditor mOscReceivePortEditor{};
    TextEditor mOscSendIpEditor{};
    TextEditor mOscSendPortEditor{};

    int mLastOscReceivePort{ 9000 };
    int mLastOscSendPort{ 8000 };
    String mLastOscSendAddress{};

public:
    //==============================================================================
    InterfaceBoxComponent(GrisLookAndFeel & grisLookAndFeel);
    ~InterfaceBoxComponent() final { setLookAndFeel(nullptr); }
    //==============================================================================
    void textEditorReturnKeyPressed([[maybe_unused]] TextEditor & editor) final { unfocusAllComponents(); }
    void paint(Graphics &) final;
    void resized() final;

    void setOscOutputPluginId(int const id) { mOscOutputPluginIdEditor.setText(String(id)); }

    void setOscReceiveToggleState(bool const state);
    void setOscReceiveInputPort(int port);

    void setOscSendToggleState(bool const state);
    void setOscSendOutputAddress(String const & address);
    void setOscSendOutputPort(int port);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InterfaceBoxComponent)
};
