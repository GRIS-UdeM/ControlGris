
#include "../JuceLibraryCode/JuceHeader.h"
#include "FieldComponent.h"

//==============================================================================
FieldComponent::FieldComponent() {}

FieldComponent::~FieldComponent() {}

//this is NOT a duplicate of one of the convert functions in processor.h
FPoint FieldComponent::convertSourceRT(float r, float t) {
	const int fieldWidth = getWidth();
	FPoint p(r * cosf(t), r * sinf(t));
	return FPoint((((p.x + kRadiusMax) / (kRadiusMax * 2)) * (fieldWidth - kSourceDiameter) + kSourceRadius), 
                     fieldWidth - (((p.y + kRadiusMax) / (kRadiusMax * 2)) * (fieldWidth - kSourceDiameter) + kSourceRadius));
}

Point <float> FieldComponent::degreeToXy(Point <float> p, int p_iFieldWidth) {
    float x, y;
    x = -((p_iFieldWidth - kSourceDiameter) / 2) * sinf(degreeToRadian(p.getX())) * cosf(degreeToRadian(p.getY()));
    y = -((p_iFieldWidth - kSourceDiameter) / 2) * cosf(degreeToRadian(p.getX())) * cosf(degreeToRadian(p.getY()));
    return Point <float> (x, y);
}

void FieldComponent::paint(Graphics& g) {
	const int fieldWidth = getWidth();
	const int fieldHeight = getHeight();
    float fFieldCenter = fieldWidth / 2;
    float w, x;
	
    g.setColour(mGrisFeel.getFieldColour());
	g.fillRect(0, 0, fieldWidth, fieldHeight);
	
    // - - - - - - - - - - - -
	// draw big background circles
	// - - - - - - - - - - - -
	g.setColour(mGrisFeel.getLightColour());
    int iCurRadius = 1;
	for (; iCurRadius <= kRadiusMax; ++iCurRadius) {
		w = (iCurRadius / kRadiusMax) * (fieldWidth - kSourceDiameter);
		x = (fieldWidth - w) / 2;
		g.drawEllipse(x, x, w, w, 1);
	}
    
    // - - - - - - - - - - - -
    // draw small, center background circles
    // - - - - - - - - - - - -
    w = (0.025 / kRadiusMax) * (fieldWidth - kSourceDiameter);
    x = (fieldWidth - w) / 2;
    g.drawEllipse(x, x, w, w, 1);
		
	// - - - - - - - - - - - -
	// draw the grid
	// - - - - - - - - - - - -
	if (true) {
		g.setColour(Colour::fromRGB(55, 56, 57));
		const int gridCount = 8;
		for (int i = 1; i < gridCount; i++) {
			g.drawLine(fieldWidth * i / gridCount, 0, fieldHeight * i / gridCount, fieldHeight);
			g.drawLine(0, fieldHeight * i / gridCount, fieldWidth, fieldHeight * i / gridCount);
		}
        g.drawLine(0, 0, fieldHeight , fieldHeight);
        g.drawLine(0, fieldHeight, fieldHeight , 0);
	}
    
    // - - - - - - - - - - - -
    //draw cross if in kOscSpatMode
    // - - - - - - - - - - - -
    g.setColour(Colours::white);
    g.drawLine(fFieldCenter, kSourceRadius, fFieldCenter, fieldHeight-kSourceRadius);
    g.drawLine(kSourceRadius, fieldHeight/2, fieldWidth-kSourceRadius, fieldHeight/2);
}

void FieldComponent::mouseDown(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();
}

void FieldComponent::mouseDrag(const MouseEvent &event) {    
	Point<int> mouseLocation(event.x, event.y);
	
	int fieldWidth = getWidth();

	repaint();
}

void FieldComponent::mouseUp(const MouseEvent &event) {
    repaint();
}
