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
#include "ControlGrisConstants.h"
#include "GrisLookAndFeel.h"

class FixedPositionEditor : public Component,
                            public TableListBoxModel
{
public:
    FixedPositionEditor();
    ~FixedPositionEditor();

    int getNumRows() override;
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override;
    void resized() override;
    void paint(Graphics& g) override;

    void loadData(XmlElement *data);

    String getText (const int columnNumber, const int rowNumber) const;
    void setText (const int columnNumber, const int rowNumber, const String& newText);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void fixedPositionEditorCellChanged(int row, int column, double value) = 0;
        virtual void fixedPositionEditorCellDeleted(int row, int column) = 0;
        virtual void fixedPositionEditorClosed() = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    TableListBox table  { {}, this };
    Font font           { 14.0f };

    TextButton  closeButton;

    XmlElement* dataList = nullptr;
    int numRows = 0;

    //==============================================================================
    class EditableTextCustomComponent  : public Label
    {
    public:
        EditableTextCustomComponent (FixedPositionEditor& td)
            : owner (td) {
            setEditable (false, true, false);
            setColour(Label::textColourId, Colours::black);
        }

        virtual TextEditor* createEditorComponent() override {
            TextEditor* const ed = Label::createEditorComponent();
            ed->setInputRestrictions(6, String("-.0123456789"));
            return ed;
        }

        void mouseDown (const MouseEvent& event) override {
            owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);
            Label::mouseDown (event);
        }

        void textWasEdited() override {
            owner.setText (columnId, row, getText());
        }

        void setRowAndColumn (const int newRow, const int newColumn) {
            row = newRow;
            columnId = newColumn;
            setText (owner.getText(columnId, row), dontSendNotification);
        }

    private:
        FixedPositionEditor& owner;
        int row, columnId;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FixedPositionEditor)
};
