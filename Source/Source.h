#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

static const float kRadiusMax = 2;
static const float kSourceRadius = 10;
static const float kSourceDiameter = kSourceRadius * 2;

typedef Point<float> FPoint;

class Source
{
public:
    Source();
    ~Source();

    void setId(int id);
    int getId();
    void setAzimuth(float azimuth);
    float getAzimuth();
    void setElevation(float elevation);
    float getElevation();
    void setDistance(float distance);
    float getDistance();
    void setColour(Colour col);
    Colour getColour();

private:
    int m_id;
    float m_azimuth;
    float m_elevation;
    float m_distance;
    Colour colour;
};
