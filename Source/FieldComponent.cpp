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
#include "ControlGrisConstants.h"

//==============================================================================
FieldComponent::FieldComponent()
{
    m_isPlaying = false;
}

FieldComponent::~FieldComponent() {
    setLookAndFeel(nullptr);
}

void FieldComponent::setIsPlaying(bool state) {
    m_isPlaying = state;
}

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
        m_sources[i].setColour(Colour::fromHSV(hue, 1.0, 1.0, 0.85));
    }
    repaint();
}

void FieldComponent::drawFieldBackground(Graphics& g, bool isMainField, SPAT_MODE_ENUM spatMode) {
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

Point <float> FieldComponent::posToXy(Point <float> p, int p_iwidth) {
    float effectiveWidth = p_iwidth - kSourceDiameter;
    float x = p.getX() * effectiveWidth;
    float y = p.getY() * effectiveWidth;
    return Point <float> (x, effectiveWidth - y);
}

Point <float> FieldComponent::xyToPos(Point <float> p, int p_iwidth) {
    float k2 = kSourceDiameter / 2.0;
    float half = (p_iwidth - kSourceDiameter) / 2;

    // Limits for the LBAP algorithm
    float px = p.getX() < kSourceRadius ? kSourceRadius : p.getX() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getX();
    float py = p.getY() < kSourceRadius ? kSourceRadius : p.getY() > p_iwidth-kSourceRadius ? p_iwidth-kSourceRadius : p.getY();

    float x = (px - k2 - half) / half * 0.5 + 0.5;
    float y = (py - k2 - half) / half * 0.5 + 0.5;

    return Point <float> (x, y);
}

//==============================================================================
MainFieldComponent::MainFieldComponent(AutomationManager& automan)
    : automationManager (automan) 
{
    m_spatMode = SPAT_MODE_VBAP;
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

void MainFieldComponent::setSpatMode(SPAT_MODE_ENUM spatMode) {
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

void MainFieldComponent::drawTrajectoryHandle(Graphics& g) {
    const int width = getWidth();
    bool shouldDrawTrajectoryHandle = false;
    if (m_numberOfSources == 1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING && !m_isPlaying) {
            shouldDrawTrajectoryHandle = true;
        }
    } else {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING && !m_isPlaying) {
            shouldDrawTrajectoryHandle = true;
        } else if (automationManager.getDrawingType() == TRAJECTORY_TYPE_REALTIME && automationManager.getSourceLink() == SOURCE_LINK_DELTA_LOCK) {
            shouldDrawTrajectoryHandle = true;
        }
    }

    if (shouldDrawTrajectoryHandle) {
        Point<float> rpos;
        if (m_spatMode == SPAT_MODE_VBAP) {
            rpos = degreeToXy(Point<float> {automationManager.getSource().getAzimuth(), automationManager.getSource().getElevation()}, width);
        } else {
            rpos = posToXy(automationManager.getSourcePosition(), width);
        }
        Rectangle<float> rarea (rpos.x, rpos.y, kSourceDiameter, kSourceDiameter);
        g.setColour(Colour::fromRGB(176, 176, 228));
        g.fillEllipse(rarea);
        g.setColour(Colour::fromRGB(64, 64, 128));
        g.drawEllipse(rarea, 1);
        g.setColour(Colours::white);
        g.drawFittedText(String("X"), rarea.getSmallestIntegerContainer(), Justification(Justification::centred), 1);
    }
}

void MainFieldComponent::paint(Graphics& g) {
    const int width = getWidth();

    drawFieldBackground(g, true, m_spatMode);

    // Draw recording trajectory handle (if source link *is not* Delta Lock).
    if (automationManager.getSourceLink() != SOURCE_LINK_DELTA_LOCK) {
        drawTrajectoryHandle(g);
    }

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (automationManager.getRecordingTrajectorySize() > 1) {
        Path trajectoryPath;
        automationManager.createRecordingPath(trajectoryPath);
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    if (m_isPlaying && !isMouseButtonDown() && automationManager.getDrawingType() != TRAJECTORY_TYPE_REALTIME) {
        Point<float> dpos = automationManager.getCurrentTrajectoryPoint();
        g.fillEllipse(dpos.x - 4, dpos.y - 4, 8, 8);
    }

    // Draw sources.
    for (int i = 0; i < m_numberOfSources; i++) {
        int lineThickness = (i == m_selectedSourceId) ? 3 : 1;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.75;
        Point<float> pos;
        if (m_spatMode == SPAT_MODE_VBAP) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = posToXy(m_sources[i].getPos(), width);
        }
        Rectangle<float> area (pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        area.expand(lineThickness, lineThickness);
        g.setColour(Colour(.2f, .2f, .2f, 1.f));
        g.drawEllipse(area.translated(.5f, .5f), 1.f);
        g.setGradientFill(ColourGradient(m_sources[i].getColour().withSaturation(saturation).darker(1.f), pos.x + kSourceRadius, pos.y + kSourceRadius,
                                         m_sources[i].getColour().withSaturation(saturation), pos.x, pos.y, true));
        g.fillEllipse(area);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), area.getSmallestIntegerContainer(), Justification(Justification::centred), 1);

        // Draw spanning.
        if (m_spatMode == SPAT_MODE_VBAP) {
            createSpanPathVBAP(g, i);
        } else {
            createSpanPathLBAP(g, i);
        }
   }

    // Draw recording trajectory handle (if source link *is* Delta Lock).
    if (automationManager.getSourceLink() == SOURCE_LINK_DELTA_LOCK) {
        drawTrajectoryHandle(g);
    }

}

bool MainFieldComponent::isTrajectoryHandleClicked(const MouseEvent &event) {
    int width = getWidth();
    if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING || automationManager.getDrawingType() == TRAJECTORY_TYPE_REALTIME) {
        Point<float> pos;
        if (m_spatMode == SPAT_MODE_VBAP) {
            pos = degreeToXy(Point<float> {automationManager.getSource().getAzimuth(), automationManager.getSource().getElevation()}, width);
        } else {
            pos = posToXy(automationManager.getSource().getPos(), width);
        }
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_oldSelectedSourceId = m_selectedSourceId;
            m_selectedSourceId = -1;
            if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING) {
                automationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
            } else {
                listeners.call([&] (Listener& l) { l.fieldTrajectoryHandleClicked(0); });
            }
            repaint();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void MainFieldComponent::mouseDown(const MouseEvent &event) {
    int width = getWidth();
    int height = getHeight();

    Point<int> mouseLocation(event.x, height - event.y);

    m_selectedSourceId = -2; // No selection (should be something more meaningful than -2!

    // Check if we click on the trajectory handle.
    if (automationManager.getSourceLink() == SOURCE_LINK_DELTA_LOCK) {
        if (isTrajectoryHandleClicked(event)) {
            return;
        }
    }

    // Check if we click on a new source.
    bool clickOnSource = false;
    for (int i = 0; i < m_numberOfSources; i++) {
        Point<float> pos;
        if (m_spatMode == SPAT_MODE_VBAP) {
            pos = degreeToXy(Point<float> {m_sources[i].getAzimuth(), m_sources[i].getElevation()}, width);
        } else {
            pos = posToXy(m_sources[i].getPos(), width);
        }
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_selectedSourceId = i;
            listeners.call([&] (Listener& l) { l.fieldSourcePositionChanged(m_selectedSourceId, 0); });
            clickOnSource = true;
            break;
        }
    }

    if (clickOnSource) {
        repaint();
        return;
    }

    // Check if we click on the trajectory handle.
    if (automationManager.getSourceLink() != SOURCE_LINK_DELTA_LOCK) {
        isTrajectoryHandleClicked(event);
    }
}

void MainFieldComponent::mouseDrag(const MouseEvent &event) {    
    int width = getWidth();
    int height = getHeight();

    // No selection.
    if (m_selectedSourceId == -2) {
        return;
    }

    Point<int> mouseLocation(event.x, height - event.y);

    Source *selectedSource;
    if (m_selectedSourceId == -1) {
        selectedSource = &automationManager.getSource();
    } else {
        selectedSource = &m_sources[m_selectedSourceId];
    }

    if (m_spatMode == SPAT_MODE_VBAP) {
        Point<float> pos = xyToDegree(mouseLocation.toFloat(), width);
        selectedSource->setAzimuth(pos.x);
        selectedSource->setElevationNoClip(pos.y);
    } else {
        Point<float> pos = xyToPos(mouseLocation.toFloat(), width);
        selectedSource->setX(pos.x);
        selectedSource->setY(pos.y);
    }

    if (m_selectedSourceId == -1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING) {
            automationManager.addRecordingPoint(clipRecordingPosition(event.getPosition()).toFloat());
        } else if (automationManager.getDrawingType() == TRAJECTORY_TYPE_REALTIME) {
            automationManager.sendTrajectoryPositionChangedEvent();
        }
    } else {
        listeners.call([&] (Listener& l) { l.fieldSourcePositionChanged(m_selectedSourceId, 0); });
    }

    bool needToAdjustAutomationManager = false;
    if (automationManager.getSourceLink() == SOURCE_LINK_INDEPENDENT && m_selectedSourceId == 0 &&
        automationManager.getDrawingType() == TRAJECTORY_TYPE_REALTIME) {
        needToAdjustAutomationManager = true;
    } else if (automationManager.getSourceLink() >= SOURCE_LINK_CIRCULAR &&
               automationManager.getSourceLink() < SOURCE_LINK_DELTA_LOCK &&
               automationManager.getDrawingType() == TRAJECTORY_TYPE_REALTIME) {
        needToAdjustAutomationManager = true;
    }

    if (needToAdjustAutomationManager) {
        if (m_spatMode == SPAT_MODE_VBAP) {
            automationManager.getSource().setAzimuth(m_sources[0].getAzimuth());
            automationManager.getSource().setElevation(m_sources[0].getElevation());
        } else {
            automationManager.getSource().setX(m_sources[0].getX());
            automationManager.getSource().setY(m_sources[0].getY());
        }
        automationManager.sendTrajectoryPositionChangedEvent();
    }

    repaint();
}

void MainFieldComponent::mouseUp(const MouseEvent &event) {
    if (m_selectedSourceId == -1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_DRAWING) {
            automationManager.addRecordingPoint(automationManager.getLastRecordingPoint());
            m_selectedSourceId = m_oldSelectedSourceId;
        }
        repaint();
    }
}

Point<int> MainFieldComponent::clipRecordingPosition(Point<int> pos) {
    Point<int> clipped;
    int max = getWidth() - 10;

    clipped.x = pos.x < 10 ? 10 : pos.x > max ? max : pos.x;
    clipped.y = pos.y < 10 ? 10 : pos.y > max ? max : pos.y;

    return clipped;
}

//==============================================================================
ElevationFieldComponent::ElevationFieldComponent(AutomationManager& automan) 
    : automationManager (automan) {}

ElevationFieldComponent::~ElevationFieldComponent() {}

void ElevationFieldComponent::paint(Graphics& g) {
    const int width = getWidth();
    const int height = getHeight();
    Point<float> pos;
    int lineThickness;

    drawFieldBackground(g, false);

    bool shouldDrawTrajectoryHandle = false;
    if (m_numberOfSources == 1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING && !m_isPlaying) {
            shouldDrawTrajectoryHandle = true;
        }
    } else {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING && !m_isPlaying) {
            shouldDrawTrajectoryHandle = true;
        } else if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_REALTIME && automationManager.getSourceLink() == SOURCE_LINK_ALT_DELTA_LOCK) {
            shouldDrawTrajectoryHandle = true;
        }
    }

    // Draw recording trajectory handle.
    if (shouldDrawTrajectoryHandle) {
        pos = posToXy(automationManager.getSourcePosition(), width);
        lineThickness = (m_selectedSourceId == -1) ? 3 : 1;
        Rectangle<float> rarea (10, pos.y, kSourceDiameter, kSourceDiameter);
        g.setColour(Colours::grey);
        g.drawLine(10 + kSourceRadius, pos.y + kSourceDiameter + lineThickness / 2,
                   10 + kSourceRadius, height - 5, lineThickness);
        g.setColour(Colours::grey);
        g.fillEllipse(rarea);
        g.setColour(Colour::fromRGB(64, 64, 84));
        g.drawEllipse(rarea, 1);
        g.setColour(Colours::white);
        g.drawFittedText(String("X"), rarea.getSmallestIntegerContainer(), Justification(Justification::centred), 1);
    }

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 176));
    if (automationManager.getRecordingTrajectorySize() > 1) {
        Path trajectoryPath;
        automationManager.createRecordingPath(trajectoryPath);
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    if (m_isPlaying && !isMouseButtonDown() && automationManager.getDrawingType() != TRAJECTORY_TYPE_ALT_REALTIME) {
        Point<float> dpos = automationManager.getCurrentTrajectoryPoint();
        g.fillEllipse(dpos.x - 4, dpos.y - 4, 8, 8);
    }

    // Draw sources.
    for (int i = 0; i < m_numberOfSources; i++) {
        lineThickness = (i == m_selectedSourceId) ? 3 : 1;
        float saturation = (i == m_selectedSourceId) ? 1.0 : 0.75;
        float x = (float)i / m_numberOfSources * (width - 50) + 50;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (height - 35) + 5;
        pos = Point<float> {x, y};
        Rectangle<float> area (pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        area.expand(lineThickness, lineThickness);
        g.setColour(Colour(.2f, .2f, .2f, 1.f));
        g.drawEllipse(area.translated(.5f, .5f), 1.f);
        g.setGradientFill(ColourGradient(m_sources[i].getColour().withSaturation(saturation).darker(1.f), pos.x + kSourceRadius, pos.y + kSourceRadius,
                                         m_sources[i].getColour().withSaturation(saturation), pos.x, pos.y, true));
        g.fillEllipse(area);
        g.drawLine(pos.x + kSourceRadius, pos.y + kSourceDiameter + lineThickness / 2,
                   pos.x + kSourceRadius, height - 5, lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(m_sources[i].getId()+1), area.getSmallestIntegerContainer(), Justification(Justification::centred), 1);

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

    m_selectedSourceId = -2; // No selection (should be something more meaningful than -2!

    // Check if we click on a new source.
    bool clickOnSource = false;
    for (int i = 0; i < m_numberOfSources; i++) {
        float x = (float)i / m_numberOfSources * (width - 50) + 50;
        float y = (90.0 - m_sources[i].getElevation()) / 90.0 * (height - 35) + 5;
        Point<float> pos = Point<float> {x, y};
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_selectedSourceId = i;
            listeners.call([&] (Listener& l) { l.fieldSourcePositionChanged(m_selectedSourceId, 1); });
            clickOnSource = true;
            break;
        }
    }

    if (clickOnSource) {
        repaint();
        return;
    }

    // Check if we record a trajectory.
    if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING || automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_REALTIME) {
        Point<float> pos = posToXy(automationManager.getSourcePosition(), width).withX(10.0f);
        Rectangle<float> area = Rectangle<float>(pos.x, pos.y, kSourceDiameter, kSourceDiameter);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            m_oldSelectedSourceId = m_selectedSourceId;
            m_selectedSourceId = -1;
            if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING) {
                currentRecordingPositionX = 10 + kSourceRadius;
                automationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
            } else {
                listeners.call([&] (Listener& l) { l.fieldTrajectoryHandleClicked(1); });
            }
            repaint();
        }
    }
}

void ElevationFieldComponent::mouseDrag(const MouseEvent &event) {    
    float height = getHeight();

    // No selection.
    if (m_selectedSourceId == -2) {
        return;
    }

    if (m_selectedSourceId == -1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING) {
            currentRecordingPositionX += 1;
            if (currentRecordingPositionX >= height) {
                currentRecordingPositionX = height;
                automationManager.compressTrajectoryXValues(height);
            }
            float y = event.getPosition().toFloat().y;
            y = y < 15.0 ? 15.0 : y > height - 20 ? height - 20 : y;
            automationManager.addRecordingPoint(Point<float> (currentRecordingPositionX, y));
            y = height - event.getPosition().toFloat().y;
            y = y < 15.0 ? 15.0 : y > height - 20 ? height - 20 : y;
            automationManager.setSourcePosition(xyToPos(Point<float> (10.0f, y), height));
        } else if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_REALTIME) {
            float y = height - event.y;
            y = y < 15.0 ? 15.0 : y > height - 20 ? height - 20 : y;
            automationManager.setSourcePosition(xyToPos(Point<float> (10.0f, y), height));
            automationManager.sendTrajectoryPositionChangedEvent();
        }
    } else {
        float elevation = (height - event.y - kSourceDiameter) / (height - 35) * 90.0;
        m_sources[m_selectedSourceId].setElevation(elevation);
        listeners.call([&] (Listener& l) { l.fieldSourcePositionChanged(m_selectedSourceId, 1); });
    }

    bool needToAdjustAutomationManager = false;
    if (automationManager.getSourceLink() == SOURCE_LINK_ALT_INDEPENDENT && m_selectedSourceId == 0 &&
        automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_REALTIME) {
        needToAdjustAutomationManager = true;
    } else if (automationManager.getSourceLink() >= SOURCE_LINK_ALT_FIXED_ELEVATION &&
               automationManager.getSourceLink() < SOURCE_LINK_ALT_DELTA_LOCK &&
               automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_REALTIME) {
        needToAdjustAutomationManager = true;
    }

    if (needToAdjustAutomationManager) {
        float y = m_sources[0].getElevation() / 90.0 * (height - 15) + 5;
        automationManager.setSourcePosition(xyToPos(Point<float> (10.0, y), height));
        automationManager.sendTrajectoryPositionChangedEvent();
    }

    repaint();
}

void ElevationFieldComponent::mouseUp(const MouseEvent &event) {
    if (m_selectedSourceId == -1) {
        if (automationManager.getDrawingType() == TRAJECTORY_TYPE_ALT_DRAWING) {
            automationManager.addRecordingPoint(automationManager.getLastRecordingPoint());
            m_selectedSourceId = m_oldSelectedSourceId;
        }
        repaint();
    }
}

