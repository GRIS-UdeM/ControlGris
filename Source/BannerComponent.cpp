#include "BannerComponent.h"

BannerComponent::BannerComponent() {
    setEditable(false, false, false);
    setColour(Label::backgroundColourId, Colour::fromRGB(64, 64, 64));
    setColour(Label::outlineColourId, Colour::fromRGB(16, 16, 16));
    setColour(Label::textColourId, Colour::fromRGB(255, 255, 255));
}

BannerComponent::~BannerComponent() {}
