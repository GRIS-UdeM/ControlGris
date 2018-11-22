
#include "../JuceLibraryCode/JuceHeader.h"
#include "FieldComponent.h"

//==============================================================================
FieldComponent::FieldComponent() {}

FieldComponent::~FieldComponent() {}

void FieldComponent::setSelectedSource(int selectedId) {
    m_selectedSourceId = selectedId;
    repaint();
}

void FieldComponent::setSources(Source *sources, int numberOfSources) {
    m_sources = sources;
    m_numberOfSources = numberOfSources;
    m_selectedSourceId = 0;
    for (int i = 0; i < m_numberOfSources; i++) {
        float hue = (float)i / m_numberOfSources + 0.577251;
        if (hue > 1) {
            hue -= 1;
        }
        m_sources[i].setColour(Colour::fromHSV(hue, 1.0, 1, 0.5));
    }
    listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
}

void FieldComponent::mouseUp(const MouseEvent &event) {
    repaint();
}

//==============================================================================
MainFieldComponent::MainFieldComponent() {
    m_drawElevation = true;
    setLookAndFeel(&mGrisFeel);
}

MainFieldComponent::~MainFieldComponent() {}

Point <float> MainFieldComponent::degreeToXy(Point <float> p, int p_iFieldWidth) {
    float x, y, distance;
    float effectiveWidth = p_iFieldWidth - kSourceDiameter;
    float radius = effectiveWidth / 2.0;
    if (m_drawElevation) {
        distance = (90.0 - p.getY()) / 90.0;
    } else {
        distance = p.getY();
    }
    x = radius * distance * sinf(degreeToRadian(p.getX())) + radius;
    y = radius * distance * cosf(degreeToRadian(p.getX())) + radius;
    return Point <float> (effectiveWidth - x, effectiveWidth - y);
}

Point <float> MainFieldComponent::xyToDegree(Point <float> p, int p_iFieldWidth) {
    float ang, rad;
    float half = p_iFieldWidth / 2;
    float x = (p.getX() - half) / half;
    float y = (p.getY() - half) / half;
    ang = atan2f(x, y) / M_PI * 180.0;
    if (ang < 180) {
        ang += 360.0;
    }
    rad = sqrtf(x*x + y*y);
    if (m_drawElevation) {
        rad = 90.0 - rad * 90.0;
    }
    return Point <float> (-ang, rad);
}

void MainFieldComponent::setDrawElevation(bool shouldDrawElevation) {
    m_drawElevation = shouldDrawElevation;
    repaint();
}

void MainFieldComponent::paint(Graphics& g) {
	const int fieldWidth = getWidth();
	const int fieldHeight = getHeight();
    float fieldCenter = fieldWidth / 2;
    float w, x;
	
    g.setColour(mGrisFeel.getFieldColour());
	g.fillRect(0, 0, fieldWidth, fieldHeight);
    g.setColour(Colours::black);
	g.drawRect(0, 0, fieldWidth, fieldHeight);
		
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
        g.drawLine(0, 0, fieldHeight, fieldHeight);
        g.drawLine(0, fieldHeight, fieldHeight, 0);
	}

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
    // draw cross
    // - - - - - - - - - - - -
    g.drawLine(fieldCenter, kSourceRadius, fieldCenter, fieldHeight-kSourceRadius);
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
        Point<float> pos;
        if (m_drawElevation) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, fieldWidth);
        } else {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getDistance()}, fieldWidth);
        }
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.setColour(Colours::white);
        g.drawText(String(i+1), pos.x + 1, pos.y + 1, kSourceDiameter, kSourceDiameter, Justification(Justification::centred), false);

        // - - - - - - - - -
        // draw spanning
        // - - - - - - - - -

        // Get current values in degrees.
        float azimuth = m_sources[i].getAzimuth();
        float elevation = m_sources[i].getElevation();
        float azimuthSpan = 180.f * m_sources[i].getAzimuthSpan();
        float elevationSpan = 45.0f * m_sources[i].getElevationSpan();

        // Calculate min and max elevation in degrees.
        Point<float> minElev = {azimuth, elevation - elevationSpan};
        Point<float> maxElev = {azimuth, elevation + elevationSpan};

        if (minElev.getY() < 0) {
            maxElev.setY(maxElev.getY() - minElev.getY());
            minElev.setY(0);
        }
        if (maxElev.getY() > 89.99) {
            minElev.setY(minElev.getY() + maxElev.getY() - 89.99);
            maxElev.setY(89.99);
        }

        // Convert min and max elevation to xy position.
        float halfWidth = (fieldWidth - kSourceDiameter) / 2.0f;
        Point<float> minElevPos = {-halfWidth * sinf(degreeToRadian(minElev.getX())) * (90.0f - minElev.getY()) / 90.0f,
                                    -halfWidth * cosf(degreeToRadian(minElev.getX())) * (90.0f - minElev.getY()) / 90.0f};
        Point<float> maxElevPos = {-halfWidth * sinf(degreeToRadian(maxElev.getX())) * (90.0f - maxElev.getY()) / 90.0f,
                                    -halfWidth * cosf(degreeToRadian(maxElev.getX())) * (90.0f - maxElev.getY()) / 90.0f};

        // Calculate min and max radius.
        float minRadius = sqrtf(minElevPos.getX()*minElevPos.getX() + minElevPos.getY()*minElevPos.getY());
        float maxRadius = sqrtf(maxElevPos.getX()*maxElevPos.getX() + maxElevPos.getY()*maxElevPos.getY());

        // Draw the path for spanning.
        Path myPath;
        myPath.startNewSubPath(fieldCenter + minElevPos.getX(), fieldCenter + minElevPos.getY());
        myPath.addCentredArc(fieldCenter, fieldCenter, minRadius, minRadius, 0.0,
                             degreeToRadian(-azimuth), degreeToRadian(-azimuth + azimuthSpan));
        myPath.addCentredArc(fieldCenter, fieldCenter, maxRadius, maxRadius, 0.0,
                             degreeToRadian(-azimuth+azimuthSpan), degreeToRadian(-azimuth-azimuthSpan));
        myPath.addCentredArc(fieldCenter, fieldCenter, minRadius, minRadius, 0.0,
                             degreeToRadian(-azimuth-azimuthSpan), degreeToRadian(-azimuth));
        myPath.closeSubPath();

        g.setColour(m_sources[i].getColour().withAlpha(0.1f));
        g.fillPath(myPath);
        g.setColour(m_sources[i].getColour().withAlpha(0.5f));
        PathStrokeType strokeType = PathStrokeType(1.5);
        g.strokePath(myPath, strokeType);
    }
}

void MainFieldComponent::mouseDown(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        Point<float> pos;
        if (m_drawElevation) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, fieldWidth);
        } else {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getDistance()}, fieldWidth);
        }
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_selectedSourceId = i;
            listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
            break;
        }
    }
    repaint();
}

void MainFieldComponent::mouseDrag(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();

    Point<int> mouseLocation(event.x, fieldHeight - event.y);
    Point<float> pos = xyToDegree(mouseLocation.toFloat(), fieldWidth);
    m_sources[m_selectedSourceId].setAzimuth(pos.x);
    if (m_drawElevation) {
        m_sources[m_selectedSourceId].setElevation(pos.y);
    } else {
        m_sources[m_selectedSourceId].setDistance(pos.y);
    }
    repaint();
    listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
}

//==============================================================================
ElevationFieldComponent::ElevationFieldComponent() {
    setLookAndFeel(&mGrisFeel);
}

ElevationFieldComponent::~ElevationFieldComponent() {}

void ElevationFieldComponent::paint(Graphics& g) {
	const int fieldWidth = getWidth();
	const int fieldHeight = getHeight();
	
    g.setColour(mGrisFeel.getFieldColour());
	g.fillRect(0, 0, fieldWidth, fieldHeight);
    g.setColour(Colours::black);
	g.drawRect(0, 0, fieldWidth, fieldHeight);

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
        g.drawLine(0, 0, fieldHeight, fieldHeight);
        g.drawLine(0, fieldHeight, fieldHeight, 0);
	}
    
    // - - - - - - - - - - - -
    // draw guide lines
    // - - - - - - - - - - - -
	g.setColour(mGrisFeel.getLightColour());
    g.drawVerticalLine(5, 5, fieldHeight-5);
    g.drawHorizontalLine(fieldHeight-5, 5, fieldWidth-5);

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
        float x = (float)i / m_numberOfSources * (fieldWidth - 10) + 10;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (fieldHeight - 35) + 5;
        Point<float> pos = Point<float> {x, y};
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.drawLine(pos.x + kSourceRadius, pos.y + kSourceDiameter, pos.x + kSourceRadius, fieldHeight - 5, lineThickness);
        g.setColour(Colours::white);
        g.drawText(String(i+1), pos.x + 1, pos.y + 1, kSourceDiameter, kSourceDiameter, Justification(Justification::centred), false);
    }
}

void ElevationFieldComponent::mouseDown(const MouseEvent &event) {    
	int fieldWidth = getWidth();
	int fieldHeight = getHeight();

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        float x = (float)i / m_numberOfSources * (fieldWidth - 10) + 10;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (fieldHeight - 35) + 5;
        Point<float> pos = Point<float> {x, y};
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_selectedSourceId = i;
            listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
            break;
        }
    }
    repaint();
}

void ElevationFieldComponent::mouseDrag(const MouseEvent &event) {    
	float fieldWidth = getWidth();
	float fieldHeight = getHeight();

    float elevation = (fieldHeight - event.y) / fieldHeight * 90.0;
    m_sources[m_selectedSourceId].setElevation(elevation);
    repaint();
    listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
}
