
#ifndef FIELDCOMPONENT_H_INCLUDED
#define FIELDCOMPONENT_H_INCLUDED

#include <deque>

#include "GrisLookAndFeel.h"

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
    
private:
    GrisLookAndFeel mGrisFeel;

	FPoint convertSourceRT(float r, float t);
    Point <float> degreeToXy(Point <float> p, int p_iFieldWidth);

    inline double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FieldComponent)
};

#endif  // FIELDCOMPONENT_H_INCLUDED
