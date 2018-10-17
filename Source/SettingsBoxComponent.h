# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class SettingsBoxComponent : public Component,
                             public TextEditor::Listener
{
public:
    SettingsBoxComponent();
    ~SettingsBoxComponent();

    void textEditorReturnKeyPressed(TextEditor &editor) override;
    void paint(Graphics&) override;
    void resized() override;

private:
    GrisLookAndFeel mGrisFeel;

    Label           oscFormatLabel;
    ComboBox        oscFormatCombo;

    Label           oscPortLabel;
    ComboBox        oscPortCombo;

    Label           numOfSourcesLabel;
    TextEditor      numOfSourcesEditor;

    Label           firstSourceIdLabel;
    TextEditor      firstSourceIdEditor;

    ToggleButton    clipSourceInCircle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsBoxComponent)
};
