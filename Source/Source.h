#pragma once

class Source
{
public:
    Source();
    ~Source();

    void setAzimuth(float azimuth);
    float getAzimuth();
    void setElevation(float elevation);
    float getElevation();
    void setRadius(float radius);
    float getRadius();

private:
    float m_azimuth;
    float m_elevation;
    float m_radius;
};
