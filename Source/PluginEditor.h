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
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GrisLookAndFeel.h"
#include "FieldComponent.h"
#include "BannerComponent.h"
#include "ParametersBoxComponent.h"
#include "TrajectoryBoxComponent.h"
#include "SettingsBoxComponent.h"
#include "SourceBoxComponent.h"
#include "InterfaceBoxComponent.h"
#include "Source.h"
#include "AutomationManager.h"
#include "FixedPositionEditor.h"

class ControlGrisAudioProcessorEditor : public AudioProcessorEditor,
                                        private Value::Listener,
                                        public FieldComponent::Listener,
                                        public ParametersBoxComponent::Listener,
                                        public SettingsBoxComponent::Listener,
                                        public SourceBoxComponent::Listener,
                                        public TrajectoryBoxComponent::Listener,
                                        public FixedPositionEditor::Listener,
                                        public Timer
{
public:
    ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor&,
                                     AudioProcessorValueTreeState& vts,
                                     AutomationManager& automan,
                                     AutomationManager& automanAlt);
    ~ControlGrisAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void valueChanged (Value&) override;
    void timerCallback() override;

    // FieldComponent::Listeners
    void fieldSourcePositionChanged(int sourceId) override;

    // ParametersBoxComponent::Listeners
    void parametersBoxSelectedSourceClicked() override;
    void parametersBoxLinkChanged(int parameterId, bool value) override;
    void parametersBoxParameterChanged(int parameterId, double value) override;

    // SettingsBoxComponent::Listeners
    void settingsBoxOscFormatChanged(int selectedId) override;
    void settingsBoxOscPortNumberChanged(int oscPort) override;
    void settingsBoxOscActivated(bool state) override;
    void settingsBoxNumberOfSourcesChanged(int numOfSources) override;
    void settingsBoxFirstSourceIdChanged(int firstSourceId) override;

    // SourceBoxComponent::Listeners
    void sourceBoxPlacementChanged(int value) override;
    void sourceBoxPositionChanged(int sourceNum, float angle, float rayLen) override;

    // TrajectoryBoxComponent::Listeners
    void trajectoryBoxSourceLinkChanged(int value) override;
    void trajectoryBoxSourceLinkAltChanged(int value) override;
    void trajectoryBoxTrajectoryTypeChanged(int value) override;
    void trajectoryBoxTrajectoryTypeAltChanged(int value) override;
    void trajectoryBoxDurationChanged(double duration, int mode) override;
    void trajectoryBoxNumOfCycleChanged(int value) override;
    void trajectoryBoxActivateChanged(bool value) override;
    void trajectoryBoxActivateAltChanged(bool value) override;
    void trajectoryBoxEditFixedSourceButtonClicked() override;
    void trajectoryBoxFixSourceButtonClicked() override;
    void trajectoryBoxClearButtonClicked() override;
    void trajectoryBoxClearAltButtonClicked() override;

    // FixedPositionEditor::Listeners
    void fixedPositionEditorCellChanged(int row, int column, double value);
    void fixedPositionEditorCellDeleted(int row, int column);
    void fixedPositionEditorClosed();

    void setPluginState();

private:
    ControlGrisAudioProcessor& processor;

    GrisLookAndFeel grisLookAndFeel;

    AudioProcessorValueTreeState& valueTreeState;

    AutomationManager& automationManager;
    AutomationManager& automationManagerAlt;

    FixedPositionEditor fixedPositionEditor;

    BannerComponent mainBanner;
    BannerComponent elevationBanner;
    BannerComponent trajectoryBanner;
    BannerComponent settingsBanner;

    MainFieldComponent  mainField;
    ElevationFieldComponent  elevationField;

    ParametersBoxComponent  parametersBox;

    TrajectoryBoxComponent  trajectoryBox;

    TabbedComponent configurationComponent { TabbedButtonBar::Orientation::TabsAtTop };

    SettingsBoxComponent    settingsBox;
    SourceBoxComponent      sourceBox;
    InterfaceBoxComponent   interfaceBox;

    bool m_fixedSourcesWindowVisible;
    int m_selectedSource;

    double m_lastTime;

    Value lastUIWidth, lastUIHeight;

    // These are called after a source has changed from mouse movement in a field.
    //----------------------------------------------------------------------------
    void validateSourcePositions();
    void validateSourcePositionsAlt();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlGrisAudioProcessorEditor)
};
