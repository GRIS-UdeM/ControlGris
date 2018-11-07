#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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
    void setRadius(float radius);
    float getRadius();
    void setColour(Colour col);
    Colour getColour();

private:
    int m_id;
    float m_azimuth;
    float m_elevation;
    float m_radius;
    Colour colour;
};
