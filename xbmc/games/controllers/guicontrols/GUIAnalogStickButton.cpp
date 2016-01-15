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

#include "GUIAnalogStickButton.h"
#include "guilib/LocalizeStrings.h"
#include "utils/StringUtils.h"

#include <string>

using namespace GAME;

CGUIAnalogStickButton::CGUIAnalogStickButton(const CGUIButtonControl& buttonTemplate,
                                             IConfigurationWizard* wizard,
                                             const CControllerFeature& feature,
                                             unsigned int index) :
  CGUIFeatureButton(buttonTemplate, wizard, feature, index)
{
  Reset();
}

bool CGUIAnalogStickButton::PromptForInput(CEvent& waitEvent)
{
  using namespace JOYSTICK;

  bool bInterrupted = false;

  std::string strPrompt = GetPrompt();

  if (!strPrompt.empty())
  {
    bInterrupted = DoPrompt(strPrompt, waitEvent);

    if (!bInterrupted)
      m_state = STATE::FINISHED; // Not interrupted, must have timed out
    else
      m_state = GetNextState(m_state); // Interrupted by input, proceed
  }

  return bInterrupted;
}

bool CGUIAnalogStickButton::IsFinished(void) const
{
  return m_state >= STATE::FINISHED;
}

JOYSTICK::CARDINAL_DIRECTION CGUIAnalogStickButton::GetDirection(void) const
{
  switch (m_state)
  {
    case STATE::ANALOG_STICK_UP:    return JOYSTICK::CARDINAL_DIRECTION::UP;
    case STATE::ANALOG_STICK_RIGHT: return JOYSTICK::CARDINAL_DIRECTION::RIGHT;
    case STATE::ANALOG_STICK_DOWN:  return JOYSTICK::CARDINAL_DIRECTION::DOWN;
    case STATE::ANALOG_STICK_LEFT:  return JOYSTICK::CARDINAL_DIRECTION::LEFT;
    default:
      break;
  }

  return JOYSTICK::CARDINAL_DIRECTION::UNKNOWN;
}

void CGUIAnalogStickButton::Reset(void)
{
  m_state = STATE::ANALOG_STICK_UP;
}

std::string CGUIAnalogStickButton::GetPrompt(void)
{
  std::string strPrompt;

  switch (m_state)
  {
    case STATE::ANALOG_STICK_UP:
    {
      std::string strPromptTemplate = g_localizeStrings.Get(35052); // "Move %s up..."
      strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
      break;
    }
    case STATE::ANALOG_STICK_RIGHT:
    {
      std::string strPromptTemplate = g_localizeStrings.Get(35054); // "Move %s right..."
      strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
      break;
    }
    case STATE::ANALOG_STICK_DOWN:
    {
      std::string strPromptTemplate = g_localizeStrings.Get(35053); // "Move %s down..."
      strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
      break;
    }
    case STATE::ANALOG_STICK_LEFT:
    {
      std::string strPromptTemplate = g_localizeStrings.Get(35055); // "Move %s left..."
      strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
      break;
    }
    default:
      break;
  }

  return strPrompt;
}
