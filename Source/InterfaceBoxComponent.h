# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class InterfaceBoxComponent : public Component,
                              public TextEditor::Listener
{
public:
    InterfaceBoxComponent();
    ~InterfaceBoxComponent();

    void textEditorReturnKeyPressed(TextEditor &editor) override;
    void paint(Graphics&) override;
    void resized() override;

private:
    GrisLookAndFeel mGrisFeel;

    Label           oscSourceLabel;
    ComboBox        oscSourceCombo;

    ToggleButton    enableLeapToggle;
    ToggleButton    enableJoystickToggle;
    ToggleButton    oscReceiveToggle;
    ToggleButton    oscSendToggle;

    TextEditor      oscReceiveIpEditor;
    TextEditor      oscReceivePortEditor;
    TextEditor      oscSendIpEditor;
    TextEditor      oscSendPortEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InterfaceBoxComponent)
};
