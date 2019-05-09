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
    m_oldSelectedSourceId = m_selectedSourceId = selectedId;
    repaint();
}

void FieldComponent::setSources(Source *sources, int numberOfSources) {
    m_sources = sources;
    m_numberOfSources = numberOfSources;
    m_oldSelectedSourceId = m_selectedSourceId = 0;
    for (int i = 0; i < m_numberOfSources; i++) {
        float hue = (float)i / m_numberOfSources + 0.577251;
        if (hue > 1) {
            hue -= 1;
        }
        m_sources[i].setColour(Colour::fromHSV(hue, 1.0, 1, 0.5));
    }
    repaint();
}

void FieldComponent::drawFieldBackground(Graphics& g, bool isMainField, SpatModeEnum spatMode) {
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
        g.setColour(lookAndFeel->getLightColour());
        if (spatMode == SPAT_MODE_VBAP) {
            // Draw big background circles.
            for (int i = 1; i < 3; i++) {
                float w = i / 2.0 * (width - kSourceDiameter);
                float x = (width - w) / 2;
                g.drawEllipse(x, x, w, w, 1);
            }

            // Draw center dot.
            float w = 0.0125 * (width - kSourceDiameter);
            float x = (width - w) / 2;
            g.drawEllipse(x, x, w, w, 1);
        } else {
            // Draw big background squares.
            float offset = width * 0.2;
            float size = width * 0.6;
            g.drawRect(offset, offset, size, size);
            g.drawRect(10, 10, width - 20, width - 20);
        }

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
    m_spatMode = SPAT_MODE_VBAP;
    recordTrajectory.setX(0.5f);
    recordTrajectory.setY(0.5f);
}

MainFieldComponent::~MainFieldComponent() {}

Point <float> MainFieldComponent::degreeToXy(Point <float> p, int p_iwidth) {
    float effectiveWidth = p_iwidth - kSourceDiameter;
    float radius = effectiveWidth / 2.0;
    float distance = (90.0 - p.getY()) / 90.0;
    float x = radius * distance * sinf(degreeToRadian(p.getX())) + radius;
    float y = radius * distance * cosf(degreeToRadian(p.getX())) + radius;
    return Point <float> (effectiveWidth - x, effectiveWidth - y);
}

Point <float> MainFieldComponent::xyToDegree(Point <float> p, int p_iwidth) {
    float k2 = kSourceDiameter / 2.0;
    float half = (p_iwidth - kSourceDiameter) / 2;
    float x = (p.getX() - k2 - half) / half;
    float y = (p.getY() - k2 - half) / half;
    float ang = atan2f(x, y) / M_PI * 180.0;
    if (ang <= -180) {
        ang += 360.0;
    }
    float rad = sqrtf(x*x + y*y);
    rad = 90.0 - rad * 90.0;
    return Point <float> (-ang, rad);
}

Point <float> MainFieldComponent::posToXy(Point <float> p, int p_iwidth) {
    float effectiveWidth = p_iwidth - kSourceDiameter;
    float x = p.getX() * effectiveWidth;
    float y = p.getY() * effectiveWidth;
    return Point <float> (x, effectiveWidth - y);
}

Point <float> MainFieldComponent::xyToPos(Point <float> p, int p_iwidth) {
    float k2 = kSourceDiameter / 2.0;
    float half = (p_iwidth - kSourceDiameter) / 2;

    // Limits for the LBAP algorithm
    float px = p.getX() < kSourceRadius ? kSourceRadius : p.getX() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getX();
    float py = p.getY() < kSourceRadius ? kSourceRadius : p.getY() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getY();

    float x = (px - k2 - half) / half * 0.5 + 0.5;
    float y = (py - k2 - half) / half * 0.5 + 0.5;

    return Point <float> (x, y);
}

void MainFieldComponent::setSpatMode(SpatModeEnum spatMode) {
    m_spatMode = spatMode;
    repaint();
}

void MainFieldComponent::createSpanPathVBAP(Graphics& g, int i) {
    const int width = getWidth();
    float fieldCenter = width / 2;
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


void MainFieldComponent::createSpanPathLBAP(Graphics& g, int i) {
	const int width = getWidth();
    float azimuthSpan = width * m_sources[i].getAzimuthSpan();
    float halfAzimuthSpan = azimuthSpan / 2.0f - kSourceRadius;
    float saturation = (i == m_selectedSourceId) ? 1.0 : 0.5;
    Point<float> pos = posToXy(m_sources[i].getPos(), width);

    g.setColour(m_sources[i].getColour().withSaturation(saturation).withAlpha(0.5f));
    g.drawEllipse(pos.x - halfAzimuthSpan, pos.y - halfAzimuthSpan, azimuthSpan, azimuthSpan, 1.5f);
    g.setColour(m_sources[i].getColour().withSaturation(saturation).withAlpha(0.1f));
    g.fillEllipse(pos.x - halfAzimuthSpan, pos.y - halfAzimuthSpan, azimuthSpan, azimuthSpan);
}

void MainFieldComponent::paint(Graphics& g) {
	const int width = getWidth();
    float fieldCenter = width / 2;

    drawFieldBackground(g, true, m_spatMode);

    // Draw recording trajectory.
    Point<float> rpos;
    if (m_spatMode == SPAT_MODE_VBAP) {
        rpos = degreeToXy(Point<float> {recordTrajectory.getAzimuth(), recordTrajectory.getElevation()}, width);
    } else {
        rpos = posToXy(recordTrajectory.getPos(), width);
    }
    g.setColour(Colours::grey);
    g.drawEllipse(rpos.x, rpos.y, kSourceDiameter, kSourceDiameter, 2);
    g.setColour(Colours::white);
    g.drawFittedText(String("X"), rpos.x + 1, rpos.y + 1, kSourceDiameter - 2,
                     kSourceDiameter, Justification(Justification::centred), 1);

    if (trajectoryPoints.size() > 1) {
        Path trajectoryPath;
        trajectoryPath.startNewSubPath(trajectoryPoints[0]);
        for (int i = 1; i < trajectoryPoints.size(); i++) {
            trajectoryPath.lineTo(trajectoryPoints[i]);
        }
        // If we want to close the path
        // trajectoryPath.closeSubPath();
        g.setColour(Colours::lightgrey);
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }

    // Draw sources.
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness = (i == m_selectedSourceId) ? 4 : 2;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.5;
        Point<float> pos;
        if (m_spatMode == SPAT_MODE_VBAP) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = posToXy(m_sources[i].getPos(), width);
        }
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), pos.x + 1, pos.y + 1, kSourceDiameter - 2,
                         kSourceDiameter, Justification(Justification::centred), 1);

        // Draw spanning.
        if (m_spatMode == SPAT_MODE_VBAP) {
            createSpanPathVBAP(g, i);
        } else {
            createSpanPathLBAP(g, i);
        }
   }
}

void MainFieldComponent::mouseDown(const MouseEvent &event) {    
	int width = getWidth();

    Point<float> pos;
    Rectangle<float> area;

    // Check if we click on a recording trajectory.
    if (m_spatMode == SPAT_MODE_VBAP) {
        pos = degreeToXy(Point<float> {recordTrajectory.getAzimuth(), recordTrajectory.getElevation()}, width);
    } else {
        pos = posToXy(recordTrajectory.getPos(), width);
    }
    area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
    if (area.contains(event.getMouseDownPosition().toFloat())) {
        m_oldSelectedSourceId = m_selectedSourceId;
        m_selectedSourceId = -1;
        trajectoryPoints.clear();
        repaint();
        return;
    }

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        if (m_spatMode == SPAT_MODE_VBAP) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = posToXy(m_sources[i].getPos(), width);
        }
        area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
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

    Source *selectedSource;
    if (m_selectedSourceId == -1) {
        selectedSource = &recordTrajectory;
    } else {
        selectedSource = &m_sources[m_selectedSourceId];
    }

    if (m_spatMode == SPAT_MODE_VBAP) {
        Point<float> pos = xyToDegree(mouseLocation.toFloat(), width);
        selectedSource->setAzimuth(pos.x);
        selectedSource->setElevation(pos.y);
    } else {
        Point<float> pos = xyToPos(mouseLocation.toFloat(), width);
        selectedSource->setX(pos.x);
        selectedSource->setY(pos.y);
    }
    if (m_selectedSourceId == -1) {
        trajectoryPoints.add(Point<float>(event.x, event.y));
    } else {
        listeners.call([&] (Listener& l) { l.sourcePositionChanged(m_selectedSourceId); });
    }
    repaint();
}

void MainFieldComponent::mouseUp(const MouseEvent &event) {
    if (m_selectedSourceId == -1) {
        m_selectedSourceId = m_oldSelectedSourceId;
        repaint();
    }
}

//==============================================================================
ElevationFieldComponent::ElevationFieldComponent() {}

ElevationFieldComponent::~ElevationFieldComponent() {}

void ElevationFieldComponent::paint(Graphics& g) {
	const int width = getWidth();
	const int height = getHeight();

    drawFieldBackground(g, false);

    // Draw sources.
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness = (i == m_selectedSourceId) ? 4 : 2;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.5;
        float x = (float)i / m_numberOfSources * (width - 25) + 25;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (height - 35) + 5;
        Point<float> pos = Point<float> {x, y};
        g.setColour(m_sources[i].getColour().withSaturation(saturation));
        g.drawEllipse(pos.x, pos.y, kSourceDiameter, kSourceDiameter, lineThickness);
        g.drawLine(pos.x + kSourceRadius, pos.y + kSourceDiameter + lineThickness / 2,
                   pos.x + kSourceRadius, height - 5, lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), pos.x + 1, pos.y + 1, kSourceDiameter - 2,
                         kSourceDiameter, Justification(Justification::centred), 1);

        // Draw spanning.
        float elevationSpan = 50.0f * m_sources[i].getElevationSpan();
        g.setColour(m_sources[i].getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawRect(pos.x + kSourceRadius - elevationSpan / 2, pos.y + kSourceDiameter + lineThickness / 2,
                   elevationSpan, height - 5.0f, 1.5);
        g.setColour(m_sources[i].getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillRect(pos.x + kSourceRadius - elevationSpan / 2, pos.y + kSourceDiameter + lineThickness / 2,
                   elevationSpan, height - 5.0f);
    }
}

void ElevationFieldComponent::mouseDown(const MouseEvent &event) {    
	int width = getWidth();
	int height = getHeight();

    // Check if we click on a new source.
    for (int i = 0; i < m_numberOfSources; i++) {
        float x = (float)i / m_numberOfSources * (width - 25) + 25;
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
