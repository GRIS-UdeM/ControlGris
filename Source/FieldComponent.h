
#ifndef FIELDCOMPONENT_H_INCLUDED
#define FIELDCOMPONENT_H_INCLUDED

#include <deque>

#include "GrisLookAndFeel.h"
#include "Source.h"

static const float kRadiusMax = 2;
static const float kSourceRadius = 10;
static const float kSourceDiameter = kSourceRadius * 2;

typedef Point<float> FPoint;

typedef enum {
	kNoSelection,
	kSelectedSource,
	kSelectedSpeaker
} SelectionType;

class FieldComponent : public Component
{
public:
    FieldComponent();
    ~FieldComponent();
    
    void paint (Graphics&);
	
	void mouseDown (const MouseEvent &event);
 	void mouseDrag (const MouseEvent &event);
 	void mouseUp (const MouseEvent &event);

    void enable(bool shouldBeEnabled);

    void setSources(Source *sources, int numberOfSources);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void sourcePositionChanged(int sourceId) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    GrisLookAndFeel mGrisFeel;

	FPoint convertSourceRT(float r, float t);
    Point <float> degreeToXy(Point <float> p, int p_iFieldWidth);
    Point <float> xyToDegree(Point <float> p, int p_iFieldWidth);

    inline double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }

    Source *m_sources;
    int m_numberOfSources;
    int m_selectedSourceId;

    bool m_enabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FieldComponent)
};

#endif  // FIELDCOMPONENT_H_INCLUDED
