/*
 *      Copyright (C) 2016 Team Kodi
 *      http://kodi.tv
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GUIFeatureButton.h"
#include "games/controllers/windows/GUIControllerDefines.h"
#include "guilib/GUIMessage.h"
#include "guilib/WindowIDs.h"
#include "ApplicationMessenger.h"
#include "threads/Event.h"

using namespace GAME;

CGUIFeatureButton::CGUIFeatureButton(const CGUIButtonControl& buttonTemplate,
                                     IConfigurationWizard* wizard,
                                     const CControllerFeature& feature,
                                     unsigned int index) :
  CGUIButtonControl(buttonTemplate),
  m_feature(feature),
  m_wizard(wizard)
{
  // Initialize CGUIButtonControl
  SetLabel(m_feature.Label());
  SetID(CONTROL_FEATURE_BUTTONS_START + index);
  SetVisible(true);
  AllocResources();
}

void CGUIFeatureButton::OnUnFocus(void)
{
  if (m_wizard->IsPrompting(this))
    m_wizard->Abort(false);
}

bool CGUIFeatureButton::DoPrompt(const std::string& strPrompt, CEvent& waitEvent)
{
  bool bInterrupted = false;

  CGUIMessage msgFocus(GUI_MSG_SETFOCUS, GetID(), GetID());
  CGUIMessage msgLabel(GUI_MSG_LABEL_SET, GetID(), GetID());

  // Acquire focus
  CApplicationMessenger::Get().SendGUIMessage(msgFocus, WINDOW_INVALID, false);

  // Set label
  msgLabel.SetLabel(strPrompt);
  CApplicationMessenger::Get().SendGUIMessage(msgLabel, WINDOW_INVALID, false);

  bInterrupted = waitEvent.Wait();

  // Reset label
  msgLabel.SetLabel(m_feature.Label());
  CApplicationMessenger::Get().SendGUIMessage(msgLabel, WINDOW_INVALID, false);

  return bInterrupted;
}
