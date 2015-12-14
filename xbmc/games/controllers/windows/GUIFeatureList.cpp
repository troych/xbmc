/*
 *      Copyright (C) 2014-2015 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GUIFeatureList.h"
#include "GUIConfigurationWizard.h"
#include "GUIControllerWindow.h"
#include "GUIFeatureButton.h"
#include "GUIGameDefines.h"
#include "games/controllers/Controller.h"
#include "guilib/GUIButtonControl.h"
#include "guilib/GUIControlGroupList.h"
#include "guilib/GUIWindow.h"

using namespace GAME;

CGUIFeatureList::CGUIFeatureList(CGUIControllerWindow* window) :
  m_guiList(nullptr),
  m_guiButtonTemplate(nullptr),
  m_window(window),
  m_focusedFeature(0)
{
  m_wizard = new CGUIConfigurationWizard(this);
}

CGUIFeatureList::~CGUIFeatureList(void)
{
  Deinitialize();
  delete m_wizard;
}

bool CGUIFeatureList::Initialize(void)
{
  m_guiList = dynamic_cast<CGUIControlGroupList*>(m_window->GetControl(CONTROL_FEATURE_LIST));
  m_guiButtonTemplate = dynamic_cast<CGUIButtonControl*>(m_window->GetControl(CONTROL_FEATURE_BUTTON_TEMPLATE));

  if (m_guiButtonTemplate)
    m_guiButtonTemplate->SetVisible(false);

  return m_guiList && m_guiButtonTemplate;
}

void CGUIFeatureList::Deinitialize(void)
{
  OnUnfocus();
  CleanupButtons();

  m_guiList = nullptr;
  m_guiButtonTemplate = nullptr;
}

void CGUIFeatureList::Load(const ControllerPtr& controller)
{
  CleanupButtons();

  m_controller = controller;
  m_focusedFeature = 0;

  const std::vector<CControllerFeature>& features = controller->Layout().Features();

  for (unsigned int buttonIndex = 0; buttonIndex < features.size() && buttonIndex < MAX_FEATURE_COUNT; buttonIndex++)
  {
    // Add to resources
    m_buttons.push_back(new CGUIFeatureButton(m_window, features[buttonIndex], controller->ID(), buttonIndex));

    // Add to GUI
    CGUIButtonControl* pButton = new CGUIButtonControl(*m_guiButtonTemplate);
    pButton->SetLabel(features[buttonIndex].Label());
    pButton->SetID(CONTROL_FEATURE_BUTTONS_START + buttonIndex);
    pButton->SetVisible(true);
    pButton->AllocResources();
    m_guiList->AddControl(pButton);
  }
}

void CGUIFeatureList::OnFocus(unsigned int index)
{
  if (m_focusedFeature == index)
    return; // Already focused

  OnUnfocus();

  m_focusedFeature = index;
}

void CGUIFeatureList::OnSelect(unsigned int index)
{
  if (index < m_buttons.size())
    m_wizard->Run(index);
}

void CGUIFeatureList::OnUnfocus(void)
{
  m_wizard->Abort();
}

bool CGUIFeatureList::PromptForInput(unsigned int featureIndex)
{
  if (featureIndex != m_focusedFeature)
  {
    m_focusedFeature = featureIndex;
    m_window->FocusFeature(featureIndex);
  }

  if (featureIndex < m_buttons.size())
    return m_buttons[featureIndex]->PromptForInput();

  return false;
}

void CGUIFeatureList::AbortPrompt(void)
{
  for (std::vector<IFeatureButton*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    (*it)->Abort();
}

void CGUIFeatureList::CleanupButtons(void)
{
  // Clear resources
  for (IFeatureButton* button : m_buttons)
    delete button;
  m_buttons.clear();

  // Clear GUI
  if (m_guiList)
    m_guiList->ClearAll();
}
