#include "SettingsBoxComponent.h"

SettingsBoxComponent::SettingsBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    oscFormatLabel.setText("OSC Format:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscFormatLabel);

    addAndMakeVisible(&oscFormatCombo);

    oscPortLabel.setText("OSC Port:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscPortLabel);

    addAndMakeVisible(&oscPortCombo);

    numOfSourcesLabel.setText("Number of Sources:", NotificationType::dontSendNotification);
    addAndMakeVisible(&numOfSourcesLabel);

    numOfSourcesEditor.setText("8");
    numOfSourcesEditor.setInputRestrictions(2, "0123456789");
    numOfSourcesEditor.addListener(this);
    addAndMakeVisible(&numOfSourcesEditor);

    firstSourceIdLabel.setText("First Source ID:", NotificationType::dontSendNotification);
    addAndMakeVisible(&firstSourceIdLabel);

    firstSourceIdEditor.setText("1");
    firstSourceIdEditor.setInputRestrictions(2, "0123456789");
    firstSourceIdEditor.addListener(this);
    addAndMakeVisible(&firstSourceIdEditor);

    clipSourceInCircle.setButtonText("Clip Sources Inside Circle");
    addAndMakeVisible(&clipSourceInCircle);
}

SettingsBoxComponent::~SettingsBoxComponent() {
    setLookAndFeel(nullptr);
}

void SettingsBoxComponent::textEditorReturnKeyPressed(TextEditor &editor) {
    unfocusAllComponents();
}

void SettingsBoxComponent::paint(Graphics& g) {}

void SettingsBoxComponent::resized() {
    double width = getWidth();

    oscFormatLabel.setBounds(5, 10, 150, 15);
    oscFormatCombo.setBounds(120, 10, 150, 20);

    oscPortLabel.setBounds(5, 40, 150, 15);
    oscPortCombo.setBounds(120, 40, 150, 20);

    numOfSourcesLabel.setBounds(330, 10, 150, 15);
    numOfSourcesEditor.setBounds(width - 50, 10, 40, 15);

    firstSourceIdLabel.setBounds(330, 40, 150, 15);
    firstSourceIdEditor.setBounds(width - 50, 40, 40, 15);

    clipSourceInCircle.setBounds(5, 105, 200, 20);
}
