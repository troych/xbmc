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

#include "GUIControllerList.h"
#include "GUIFeatureList.h"
#include "GUIGameDefines.h"
#include "games/controllers/Controller.h"
#include "games/controllers/ControllerManager.h"
#include "guilib/GUIButtonControl.h"
#include "guilib/GUIControlGroupList.h"
#include "guilib/GUIWindow.h"

using namespace GAME;

CGUIControllerList::CGUIControllerList(CGUIWindow* window, IFeatureList* featureList) :
  m_window(window),
  m_featureList(featureList),
  m_controllerList(nullptr),
  m_controllerButton(nullptr)
{
}

bool CGUIControllerList::Initialize(void)
{
  m_controllerList = dynamic_cast<CGUIControlGroupList*>(m_window->GetControl(CONTROL_CONTROLLER_LIST));
  m_controllerButton = dynamic_cast<CGUIButtonControl*>(m_window->GetControl(CONTROL_CONTROLLER_BUTTON_TEMPLATE));

  if (m_controllerButton)
    m_controllerButton->SetVisible(false);

  return m_controllerList != nullptr &&
         m_controllerButton != nullptr;
}

void CGUIControllerList::Deinitialize(void)
{
  CleanupButtons();

  m_controllerList = nullptr;
  m_controllerButton = nullptr;
}

void CGUIControllerList::Refresh(void)
{
  CleanupButtons();

  m_controllers = CControllerManager::GetInstance().GetControllers();

  unsigned int buttonId = 0;
  for (const ControllerPtr& controller : m_controllers)
  {
    CGUIButtonControl* pButton = new CGUIButtonControl(*m_controllerButton);
    pButton->SetLabel(controller->Label());
    pButton->SetID(CONTROL_CONTROLLER_BUTTONS_START + buttonId++);
    pButton->SetVisible(true);
    pButton->AllocResources();
    m_controllerList->AddControl(pButton);

    // Just in case
    if (buttonId >= MAX_CONTROLLER_COUNT)
      break;
  }
}

void CGUIControllerList::OnFocus(unsigned int controllerIndex)
{
  if (controllerIndex < m_controllers.size())
    m_featureList->Load(m_controllers[controllerIndex]);
}

void CGUIControllerList::OnSelect(unsigned int controllerIndex)
{
  if (controllerIndex < m_controllers.size())
  {
    // TODO
  }
}

void CGUIControllerList::CleanupButtons(void)
{
  if (m_controllerList)
    m_controllerList->ClearAll();
}
