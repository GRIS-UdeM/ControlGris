/*
  ==============================================================================

    SourceComponent.cpp
    Created: 30 May 2020 11:52:24pm
    Author:  samuel

  ==============================================================================
*/

#include "SourceComponent.h"

#include "FieldComponent.h"
#include "Source.h"

SourceComponent::SourceComponent(FieldComponent & fieldComponent, Source & source)
    : mFieldComponent(fieldComponent)
    , mSource(source)
{
    source.addChangeListener(this);
    constexpr auto dimension = SOURCE_FIELD_COMPONENT_DIAMETER + MAX_LINE_THICKNESS * 2;
    this->setSize(dimension, dimension);
    this->updatePositionInParent();
}

SourceComponent::~SourceComponent()
{
    mSource.removeChangeListener(this);
}

void SourceComponent::paint(Graphics & g)
{
    auto const sourceId{ mSource.getId() };
    auto const selectedSourceId{ mFieldComponent.getSelectedSourceId() };

    int const lineThickness{ (sourceId == selectedSourceId) ? MAX_LINE_THICKNESS : 1 };
    float const saturation{ (sourceId == selectedSourceId) ? 1.0f : 0.75f };
    Rectangle<float> area{ MAX_LINE_THICKNESS,
                           MAX_LINE_THICKNESS,
                           SOURCE_FIELD_COMPONENT_DIAMETER,
                           SOURCE_FIELD_COMPONENT_DIAMETER };
    area.expand(lineThickness, lineThickness);
    g.setColour(Colour(.2f, .2f, .2f, 1.0f));
    g.drawEllipse(area.translated(.5f, .5f), 1.0f);
    g.setGradientFill(ColourGradient(mSource.getColour().withSaturation(saturation).darker(1.0f),
                                     MAX_LINE_THICKNESS + SOURCE_FIELD_COMPONENT_RADIUS,
                                     MAX_LINE_THICKNESS + SOURCE_FIELD_COMPONENT_RADIUS,
                                     mSource.getColour().withSaturation(saturation),
                                     MAX_LINE_THICKNESS,
                                     MAX_LINE_THICKNESS,
                                     true));
    g.fillEllipse(area);
    g.setColour(Colours::white);
    g.drawFittedText(String{ sourceId + 1 }, area.getSmallestIntegerContainer(), Justification::centred, 1);
}

void SourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourcePositionToComponentPosition(mSource.getPos()) };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

void SourceComponent::mouseDown(MouseEvent const & event)
{
    auto const sourceLink{ mFieldComponent.getAutomationManager().getSourceLink() };

    auto * mainFieldComponent{ dynamic_cast<MainFieldComponent *>(&mFieldComponent) }; // TODO: dont do this

    if (mainFieldComponent != nullptr && mSource.getId() > 0 && sourceLink != PositionSourceLink::independent
        && sourceLink != PositionSourceLink::circularDeltaLock) {
        mainFieldComponent->showCircularSourceSelectionWarning();
    } else {
        this->setSourcePosition(event);
        mFieldComponent.setSelectedSource(mSource.getId());
    }
}

void SourceComponent::setSourcePosition(MouseEvent const & event)
{
    jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newPosition{ mFieldComponent.componentPositionToSourcePosition(eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setPos(newPosition);

    mFieldComponent.notifySourcePositionChanged(mSource.getId());
}

void SourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceId() == mSource.getId()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

        this->setSourcePosition(event);

        repaint();

        // TODO: what if trajectory handle?
    }
}

void SourceComponent::mouseUp(MouseEvent const & event)
{
    // TODO: what if trajectory handle?
    // TODO: maybe turn off circularSourceSelectionWarning?
}

void SourceComponent::changeListenerCallback(ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
