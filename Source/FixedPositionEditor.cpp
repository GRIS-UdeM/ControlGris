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
#include "FixedPositionEditor.h"
#include "ControlGrisUtilities.h"

FixedPositionEditor::FixedPositionEditor() {
    table.setModel(this);
    addAndMakeVisible (table);

    int index = 0;

    table.getHeader().addColumn(FIXED_POSITION_DATA_HEADERS[index++], 1, 40, 40, 40, TableHeaderComponent::notSortable);
    table.getHeader().addColumn(FIXED_POSITION_DATA_HEADERS[index++], 2, 80, 60, 100, TableHeaderComponent::notSortable);
    for (int i = 1; i <= 8; i++) {
        table.getHeader().addColumn(FIXED_POSITION_DATA_HEADERS[index++], i*3, 50, 50, 75, TableHeaderComponent::notSortable);
        table.getHeader().addColumn(FIXED_POSITION_DATA_HEADERS[index++], i*3+1, 50, 50, 75, TableHeaderComponent::notSortable);
        table.getHeader().addColumn(FIXED_POSITION_DATA_HEADERS[index++], i*3+2, 50, 50, 75, TableHeaderComponent::notSortable);
    }

    table.setMultipleSelectionEnabled (false);

    addAndMakeVisible(&closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this] { listeners.call([&] (Listener& l) { l.fixedPositionEditorClosed(); }); };

}

FixedPositionEditor::~FixedPositionEditor() {
    setLookAndFeel(nullptr);
}

int FixedPositionEditor::getNumRows() {
    return numRows;
}

void FixedPositionEditor::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());

    if (rowIsSelected) {
        g.fillAll(lookAndFeel->getHighlightColour());
    } else {
        if (rowNumber % 2)
            g.fillAll(lookAndFeel->getBackgroundColour().withBrightness(0.6));
        else
            g.fillAll(lookAndFeel->getBackgroundColour().withBrightness(0.7));
    }
}

void FixedPositionEditor::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    g.setColour (Colours::black);
    g.setFont (font);

    if (auto* rowElement = dataList->getChildElement (rowNumber)) {
        String text = getText(columnId, rowNumber);
        g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour(Colours::black.withAlpha (0.2f));
    g.fillRect (width - 1, 0, 1, height);
}

Component * FixedPositionEditor::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                                          Component* existingComponentToUpdate) {

    if (columnId == 1) {
        GrisLookAndFeel *lookAndFeel;
        lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
        TextButton *tbRemove = static_cast<TextButton *> (existingComponentToUpdate);
        if (tbRemove == nullptr)
            tbRemove = new TextButton();
        tbRemove->setButtonText("X");
        tbRemove->setName(String(rowNumber));
        tbRemove->setBounds(5, 5, 30, 30);
        tbRemove->setColour(ToggleButton::textColourId, lookAndFeel->getFontColour());
        tbRemove->setLookAndFeel(lookAndFeel);
        tbRemove->onClick = [this, rowNumber, columnId] { 
            listeners.call([&] (Listener& l) { l.fixedPositionEditorCellDeleted(rowNumber, columnId); });    
        };
        return tbRemove;
    }

    auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

    if (textLabel == nullptr)
        textLabel = new EditableTextCustomComponent (*this);

    textLabel->setRowAndColumn (rowNumber, columnId);
    return textLabel;
}

void FixedPositionEditor::resized() {
    table.setBounds(1, 1, getWidth() - 1, getHeight() - 35);
    closeButton.setBounds(5, getHeight() - 30, getWidth() - 10, 25);
}

void FixedPositionEditor::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());

    table.setColour (ListBox::textColourId, Colour::fromRGB(0, 0, 0));
    table.setColour (ListBox::backgroundColourId, lookAndFeel->findColour (ResizableWindow::backgroundColourId));
    table.setColour (ListBox::outlineColourId, Colour::fromRGB(16, 16, 16));
    table.setOutlineThickness (1);

    table.getHeader().setColour (TableHeaderComponent::textColourId, Colour::fromRGB(255, 255, 255));
    table.getHeader().setColour (TableHeaderComponent::backgroundColourId, Colour::fromRGB(64, 64, 64));
    table.getHeader().setColour (TableHeaderComponent::outlineColourId, Colour::fromRGB(16, 16, 16));
}

void FixedPositionEditor::loadData(XmlElement *data) {
    dataList = data;
    numRows = dataList->getNumChildElements();
    table.updateContent();
}

String FixedPositionEditor::getText (const int columnNumber, const int rowNumber) const {
    return String(dataList->getChildElement(rowNumber)->getDoubleAttribute(table.getHeader().getColumnName(columnNumber)), 3);
}

void FixedPositionEditor::setText (const int columnNumber, const int rowNumber, const String& newText) {
    float val = getFloatPrecision(newText.getFloatValue(), 3);
    listeners.call([&] (Listener& l) { l.fixedPositionEditorCellChanged(rowNumber, columnNumber, val); });    
}
