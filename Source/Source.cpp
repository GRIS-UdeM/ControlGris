#include "Source.h"

Source::Source() {
    m_azimuth = 0.0;
    m_elevation = 0.0;
    m_radius = 1.0;
}

void Source::setAzimuth(float azimuth) {
    m_azimuth = azimuth;
}

float Source::getAzimuth() {
    return m_azimuth;
}

void Source::setElevation(float elevation) {
    if (elevation < 0.0) {
        m_elevation = 0.0;
    } else if (elevation > 90.0) {
        m_elevation = 90.0;
    } else {
        m_elevation = elevation;
    }
}

float Source::getElevation() {
    return m_elevation;
}

void Source::setRadius(float radius) {
    m_radius = radius;
}

float Source::getRadius() {
    return m_radius;
}

Source::~Source() {}
