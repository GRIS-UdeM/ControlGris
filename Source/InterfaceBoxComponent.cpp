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
}

void InterfaceBoxComponent::textEditorReturnKeyPressed(TextEditor &editor) {
    unfocusAllComponents();
}

void InterfaceBoxComponent::paint(Graphics& g) {}

void InterfaceBoxComponent::resized() {
    double width = getWidth();

    oscSourceLabel.setBounds(5, 5, 150, 15);
    oscSourceCombo.setBounds(120, 5, 150, 20);

    enableLeapToggle.setBounds(5, 55, 200, 20);
    enableJoystickToggle.setBounds(5, 85, 200, 20);

    oscReceiveToggle.setBounds(205, 55, 200, 20);
    oscSendToggle.setBounds(205, 85, 200, 20);

    oscReceiveIpEditor.setBounds(355, 55, 120, 20);
    oscReceivePortEditor.setBounds(480, 55, 60, 20);

    oscSendIpEditor.setBounds(355, 85, 120, 20);
    oscSendPortEditor.setBounds(480, 85, 60, 20);
}
