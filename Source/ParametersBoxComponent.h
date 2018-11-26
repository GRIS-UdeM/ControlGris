# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"
#include "Source.h"

//-------------------------------------------------------------------
class ParameterComponent : public Component,
                           public Button::Listener,
                           public Slider::Listener
{
public:
    ParameterComponent(int ParameterId, String label, Component *parent);
    ~ParameterComponent();

    void buttonClicked(Button *button) override;
    void sliderValueChanged(Slider *slider) override;
    void paint(Graphics&) override;
    void resized() override;

    void setValue(double value);
    bool getLinkState();
    void setLinkState(bool state);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void parameterChanged(int parameterId, double value) = 0;
        virtual void parameterLinkChanged(int parameterId, bool value) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    int m_parameterId;

    Label           parameterLabel;
    ToggleButton    linkButton;
    Slider          slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterComponent)
};

//-----------------------------------------------------------------

class ParametersBoxComponent : public Component,
                               public Button::Listener,
                               public ParameterComponent::Listener
{
public:
    ParametersBoxComponent();
    ~ParametersBoxComponent();

    void buttonClicked(Button *button) override;
    void parameterChanged(int parameterId, double value) override;
    void parameterLinkChanged(int parameterId, bool value) override;
    void paint(Graphics&) override;
    void resized() override;

    void setSelectedSource(Source *source);
    void setDistanceEnabled(bool shouldBeEnabled);
    bool getLinkState(int parameterId);
    void setLinkState(int parameterId, bool state);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void parameterChanged(int parameterId, double value) = 0;
        virtual void parameterLinkChanged(int parameterId, bool value) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    GrisLookAndFeel mGrisFeel;

    bool m_distanceEnabled;

    Source *selectedSource;

    ParameterComponent p_azimuth;
    ParameterComponent p_elevation;
    ParameterComponent p_distance;
    ParameterComponent p_x;
    ParameterComponent p_y;
    ParameterComponent p_azimuthSpan;
    ParameterComponent p_elevationSpan;

    ToggleButton activatorXY;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametersBoxComponent)
};
