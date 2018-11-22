#include "Source.h"

Source::Source() {
    m_azimuth = 0.0;
    m_elevation = 0.0;
    m_distance = 1.0;
    m_aziSpan = 0.0;
    m_eleSpan = 0.0;
    colour = Colours::black;
}

Source::~Source() {}

void Source::setId(int id) {
    m_id = id;
}

int Source::getId() {
    return m_id;
}

void Source::setAzimuth(float azimuth) {
    m_azimuth = azimuth;
}

void Source::setNormalizedAzimuth(float value) {
    if (value < 0.5) {
        m_azimuth = value * 360;
    } else {
        m_azimuth = (value - 1.0) * 360;
    }
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

void Source::setDistance(float distance) {
    m_distance = distance;
}

float Source::getDistance() {
    return m_distance;
}

void Source::setAzimuthSpan(float azimuthSpan) {
    m_aziSpan = azimuthSpan;
}

float Source::getAzimuthSpan() {
    return m_aziSpan;
}

void Source::setElevationSpan(float elevationSpan) {
    m_eleSpan = elevationSpan;
}

float Source::getElevationSpan() {
    return m_eleSpan;
}

void Source::setColour(Colour col) {
    colour = col;
}

Colour Source::getColour() {
    return colour;
}