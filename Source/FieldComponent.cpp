/**************************************************************************
 * Copyright 2018 UdeM - GRIS - Olivier Belanger                          *
 *                                                                        *
 * This file is part of ControlGris, a multi-source spatialization plugin *
 *                                                                        *
 * ControlGris is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Lesser General Public License as         *
 * published by the Free Software Foundation, either version 3 of the     *
 * License, or (at your option) any later version.                        *
 *                                                                        *
 * ControlGris is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU Lesser General Public License for more details.                    *
 *                                                                        *
 * You should have received a copy of the GNU Lesser General Public       *
 * License along with ControlGris.  If not, see                           *
 * <http://www.gnu.org/licenses/>.                                        *
 *************************************************************************/
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
    repaint();
}

void FieldComponent::drawFieldBackground(Graphics& g, bool isMainField) {
	const int width = getWidth();
	const int height = getHeight();
    float fieldCenter = width / 2;

    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());

	// Draw the background.
    g.setColour(lookAndFeel->getFieldColour());
	g.fillRect(0, 0, width, height);
    g.setColour(Colours::black);
	g.drawRect(0, 0, width, height);
		
	// Draw the grid.
	if (true) {
		g.setColour(Colour::fromRGB(55, 56, 57));
		const int gridCount = 8;
		for (int i = 1; i < gridCount; i++) {
			g.drawLine(width * i / gridCount, 0, height * i / gridCount, height);
			g.drawLine(0, height * i / gridCount, width, height * i / gridCount);
		}
        g.drawLine(0, 0, height, height);
        g.drawLine(0, height, height, 0);
	}

    if (isMainField) {
        // Draw big background circles.
        g.setColour(lookAndFeel->getLightColour());
        for (int i = 1; i < 3; i++) {
            float w = i / 2.0 * (width - kSourceDiameter);
            float x = (width - w) / 2;
            g.drawEllipse(x, x, w, w, 1);
        }
        
        // Draw center dot.
        float w = 0.0125 * (width - kSourceDiameter);
        float x = (width - w) / 2;
        g.drawEllipse(x, x, w, w, 1);
        
        // Draw cross.
        g.drawLine(fieldCenter, kSourceRadius, fieldCenter, height-kSourceRadius);
        g.drawLine(kSourceRadius, height/2, width-kSourceRadius, height/2);
    } else {
        // Draw guide lines
        g.setColour(lookAndFeel->getLightColour());
        g.drawVerticalLine(5, 5, height - 5);
        g.drawHorizontalLine(height - 5, 5, width - 5);
    }
}

void FieldComponent::mouseUp(const MouseEvent &event) {
    repaint();
}

//==============================================================================
MainFieldComponent::MainFieldComponent() {
    m_drawElevation = true;
}

MainFieldComponent::~MainFieldComponent() {}

Point <float> MainFieldComponent::degreeToXy(Point <float> p, int p_iwidth) {
    float x, y, distance;
    float effectiveWidth = p_iwidth - kSourceDiameter;
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

Point <float> MainFieldComponent::xyToDegree(Point <float> p, int p_iwidth) {
    float ang, rad, px, py;
    float k2 = kSourceDiameter / 2.0;
    float effectiveWidth = p_iwidth - kSourceDiameter;
    float half = effectiveWidth / 2;

    if (! m_drawElevation) {
        // Limits for the LBAP algorithm
        px = p.getX() < kSourceRadius ? kSourceRadius : p.getX() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getX();
        py = p.getY() < kSourceRadius ? kSourceRadius : p.getY() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getY();
    } else {
        px = p.getX();
        py = p.getY();
    }

    float x = (px - k2 - half) / half;
    float y = (py - k2 - half) / half;
    ang = atan2f(x, y) / M_PI * 180.0;
    if (ang <= -180) {
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
	const int width = getWidth();
    float fieldCenter = width / 2;

    drawFieldBackground(g, true);

    // Draw sources.
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness = (i == m_selectedSourceId) ? 4 : 2;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.5;
        Point<float> pos;
        if (m_drawElevation) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getDistance()}, width);
        }
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), pos.x + 1, pos.y + 1, kSourceDiameter - 2, kSourceDiameter,
                         Justification(Justification::centred), 1);

        // Draw spanning.
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
        float halfWidth = (width - kSourceDiameter) / 2.0f;
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
	int width = getWidth();

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        Point<float> pos;
        if (m_drawElevation) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getDistance()}, width);
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
	int width = getWidth();
	int height = getHeight();

    Point<int> mouseLocation(event.x, height - event.y);
    Point<float> pos = xyToDegree(mouseLocation.toFloat(), width);
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
ElevationFieldComponent::ElevationFieldComponent() {}

ElevationFieldComponent::~ElevationFieldComponent() {}

void ElevationFieldComponent::paint(Graphics& g) {
	const int width = getWidth();
	const int height = getHeight();

    drawFieldBackground(g, false);

    // Draw sources
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness = (i == m_selectedSourceId) ? 4 : 2;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.5;
        float x = (float)i / m_numberOfSources * (width - 10) + 10;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (height - 35) + 5;
        Point<float> pos = Point<float> {x, y};
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.drawLine(pos.x + kSourceRadius, pos.y + kSourceDiameter + lineThickness / 2, pos.x + kSourceRadius, height - 5, lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), pos.x + 1, pos.y + 1, kSourceDiameter - 2, kSourceDiameter,
                         Justification(Justification::centred), 1);
    }
}

void ElevationFieldComponent::mouseDown(const MouseEvent &event) {    
	int width = getWidth();
	int height = getHeight();

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        float x = (float)i / m_numberOfSources * (width - 10) + 10;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (height - 35) + 5;
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
	float height = getHeight();

    float elevation = (height - event.y - kSourceDiameter) / (height - 35) * 90.0;
    m_sources[m_selectedSourceId].setElevation(elevation);
    repaint();
    listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
}
