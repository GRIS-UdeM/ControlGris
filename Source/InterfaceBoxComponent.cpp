#include "InterfaceBoxComponent.h"

InterfaceBoxComponent::InterfaceBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    oscSourceLabel.setText("OSC Source:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscSourceLabel);

    addAndMakeVisible(&oscSourceCombo);

    enableLeapToggle.setButtonText("Enable Leap");
    addAndMakeVisible(&enableLeapToggle);

    enableJoystickToggle.setButtonText("Enable Joystick");
    addAndMakeVisible(&enableJoystickToggle);

    oscReceiveToggle.setButtonText("Receive on IP : port");
    addAndMakeVisible(&oscReceiveToggle);

    oscSendToggle.setButtonText("Send on IP : port");
    addAndMakeVisible(&oscSendToggle);

    oscReceiveIpEditor.setText("192.168.1.2");
    oscReceiveIpEditor.setInputRestrictions(16, ".0123456789");
    oscReceiveIpEditor.addListener(this);
    addAndMakeVisible(&oscReceiveIpEditor);

    oscReceivePortEditor.setText("8000");
    oscReceivePortEditor.setInputRestrictions(5, "0123456789");
    oscReceivePortEditor.addListener(this);
    addAndMakeVisible(&oscReceivePortEditor);

    oscSendIpEditor.setText("192.168.1.100");
    oscSendIpEditor.setInputRestrictions(16, ".0123456789");
    oscSendIpEditor.addListener(this);
    addAndMakeVisible(&oscSendIpEditor);

    oscSendPortEditor.setText("9000");
    oscSendPortEditor.setInputRestrictions(5, "0123456789");
    oscSendPortEditor.addListener(this);
    addAndMakeVisible(&oscSendPortEditor);
}

InterfaceBoxComponent::~InterfaceBoxComponent() {
    setLookAndFeel(nullptr);
}

void InterfaceBoxComponent::textEditorReturnKeyPressed(TextEditor &editor) {
    unfocusAllComponents();
}

void InterfaceBoxComponent::paint(Graphics& g) {}

void InterfaceBoxComponent::resized() {
    oscSourceLabel.setBounds(5, 10, 90, 20);
    oscSourceCombo.setBounds(95, 10, 150, 20);

    enableLeapToggle.setBounds(5, 35, 120, 20);
    enableJoystickToggle.setBounds(125, 35, 150, 20);

    oscReceiveToggle.setBounds(5, 65, 200, 20);
    oscSendToggle.setBounds(5, 95, 200, 20);

    oscReceiveIpEditor.setBounds(150, 65, 120, 20);
    oscReceivePortEditor.setBounds(280, 65, 60, 20);

    oscSendIpEditor.setBounds(150, 95, 120, 20);
    oscSendPortEditor.setBounds(280, 95, 60, 20);
}
