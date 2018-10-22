
#include "../JuceLibraryCode/JuceHeader.h"
#include "FieldComponent.h"

//==============================================================================
FieldComponent::FieldComponent() {
    setLookAndFeel(&mGrisFeel);
}

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
    float distance = (p_iFieldWidth - kSourceDiameter) / 2.0;
    float radius = (90.0 - p.getY()) / 90.0;
    x = distance * radius * sinf(degreeToRadian(p.getX())) + distance;
    y = distance * radius * cosf(degreeToRadian(p.getX())) + distance;
    return Point <float> (x, y);
}

Point <float> FieldComponent::xyToDegree(Point <float> p, int p_iFieldWidth) {
    float ang, rad;
    float half = p_iFieldWidth / 2;
    float x = (p.getX() - half) / half;
    float y = (p_iFieldWidth - p.getY() - half) / half;
    ang = atan2f(x, y) / M_PI * 180.0;
    if (ang < 0) {
        ang = 360.0 + ang;
    }
    rad = 90.0 - (sqrtf(x*x + y*y) * 90.0);
    return Point <float> (ang, rad);
}

//------------------------------------------------------------------------------
void FieldComponent::setSources(Source *sources, int numberOfSources) {
    m_sources = sources;
    m_numberOfSources = numberOfSources;
    m_selectedSourceId = -1;
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
    // draw cross if in kOscSpatMode
    // - - - - - - - - - - - -
    g.setColour(Colours::white);
    g.drawLine(fFieldCenter, kSourceRadius, fFieldCenter, fieldHeight-kSourceRadius);
    g.drawLine(kSourceRadius, fieldHeight/2, fieldWidth-kSourceRadius, fieldHeight/2);

    // - - - - - - - - - - - - 
    // draw sources
    // - - - - - - - - - - - - 
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness;
        float saturation;
        if (i == m_selectedSourceId) {
            lineThickness = 4;
            saturation = 1.0;
        } else {
            lineThickness = 2;
            saturation = 0.5;
        }
        float hue = (float)i / m_numberOfSources + 0.577251;
        if (hue > 1){
            hue -= 1;
        }
        Point<float> pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, fieldWidth);
        g.setColour(Colour::fromHSV(hue, saturation, 1, 0.5));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.setColour(Colours::white);
        g.drawText(String(i+1), pos.x + 1, pos.y + 1, kSourceDiameter, kSourceDiameter, Justification(Justification::centred), false);
    }
   
}

void FieldComponent::mouseDown(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();

    // Do we click on a source ?
    for (int i = 0; i < m_numberOfSources; i++) {
        Point<float> pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, fieldWidth);
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_selectedSourceId = i;
            break;
        }
    }
    repaint();
}

void FieldComponent::mouseDrag(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();

    if (m_selectedSourceId >= 0) {
        Point<int> mouseLocation(event.x, fieldHeight - event.y);	
        Point<float> pos = xyToDegree(mouseLocation.toFloat(), fieldWidth);
        m_sources[m_selectedSourceId].setAzimuth(pos.x);
        m_sources[m_selectedSourceId].setElevation(pos.y);
        repaint();
        listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
    }
}

void FieldComponent::mouseUp(const MouseEvent &event) {
    m_selectedSourceId = -1;
    repaint();
}
