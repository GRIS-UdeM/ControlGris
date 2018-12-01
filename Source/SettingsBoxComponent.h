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

class SettingsBoxComponent : public Component
{
public:
    SettingsBoxComponent();
    ~SettingsBoxComponent() {};

    void paint(Graphics&) override;
    void resized() override;

    // These are only setters, they dont send notification.
    //-----------------------------------------------------
    void setNumberOfSources(int numOfSources);
    void setFirstSourceId(int firstSourceId);
    void setOscFormat(int oscFormatIndex);
    void setOscPortNumber(int oscPortNumber);
    void setActivateButtonState(bool shouldBeOn);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void oscFormatChanged(int selectedId) = 0;
        virtual void oscPortNumberChanged(int oscPort) = 0;
        virtual void oscActivated(bool state) = 0;
        virtual void numberOfSourcesChanged(int numOfSources) = 0;
        virtual void firstSourceIdChanged(int firstSourceId) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    Label           oscFormatLabel;
    ComboBox        oscFormatCombo;

    Label           oscPortLabel;
    TextEditor      oscPortEditor;

    Label           numOfSourcesLabel;
    TextEditor      numOfSourcesEditor;

    Label           firstSourceIdLabel;
    TextEditor      firstSourceIdEditor;

    ToggleButton    activateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsBoxComponent)
};
