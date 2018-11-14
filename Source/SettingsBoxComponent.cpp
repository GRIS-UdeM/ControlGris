#include "SettingsBoxComponent.h"

SettingsBoxComponent::SettingsBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    oscFormatLabel.setText("OSC Format:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscFormatLabel);

    oscFormatCombo.addItem("VBAP - ServerGris", 1);
    oscFormatCombo.addItem("LBAP - ServerGris", 2);
    oscFormatCombo.onChange = [this] { onNewOscFormat(); };
    oscFormatCombo.setSelectedId(1);
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

void SettingsBoxComponent::onNewOscFormat() {
    listeners.call([&] (Listener& l) { l.oscFormatChanged(oscFormatCombo.getSelectedId()); });
}

void SettingsBoxComponent::paint(Graphics& g) {}

void SettingsBoxComponent::resized() {
    double width = getWidth();

    oscFormatLabel.setBounds(5, 10, 90, 15);
    oscFormatCombo.setBounds(95, 10, 150, 20);

    oscPortLabel.setBounds(5, 40, 90, 15);
    oscPortCombo.setBounds(95, 40, 150, 20);

    numOfSourcesLabel.setBounds(265, 10, 130, 15);
    numOfSourcesEditor.setBounds(395, 10, 40, 15);

    firstSourceIdLabel.setBounds(265, 40, 130, 15);
    firstSourceIdEditor.setBounds(395, 40, 40, 15);

    clipSourceInCircle.setBounds(5, 105, 200, 20);
}
