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

    void setChanged(bool changed);
    bool getChanged();

    void setRadiusIsElevation(bool shouldBeElevation);

    void setAzimuth(float azimuth);
    void setNormalizedAzimuth(float value);
    float getAzimuth();
    void setElevation(float elevation);
    float getElevation();
    void setDistance(float distance);
    float getDistance();
    void setAzimuthSpan(float azimuthSpan);
    float getAzimuthSpan();
    void setElevationSpan(float elevationSpan);
    float getElevationSpan();

    void setX(float x);
    float getX();
    void setY(float y);
    float getY();

    void computeXY();
    void computeAzimuthElevation();

    void setColour(Colour col);
    Colour getColour();

private:
    int m_id;
    bool m_changed;
    bool m_radiusIsElevation;
    float m_azimuth;
    float m_elevation;
    float m_distance;
    float m_aziSpan;
    float m_eleSpan;
    float m_x;
    float m_y;
    Colour colour;

    inline double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }

};
