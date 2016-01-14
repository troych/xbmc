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

#include "GUIFeatureButton.h"
#include "GUIControllerDefines.h"
#include "games/controllers/Controller.h"
#include "games/controllers/ControllerFeature.h"
#include "guilib/GUIButtonControl.h"
#include "guilib/GUIControlGroupList.h"
#include "guilib/GUIWindow.h"
#include "guilib/LocalizeStrings.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "utils/StringUtils.h"

using namespace GAME;
using namespace JOYSTICK;

#define ESC_KEY_CODE  27

CGUIFeatureButton::CGUIFeatureButton(IConfigurationWindow* window,
                                     const CControllerFeature& feature,
                                     const std::string& strControllerId,
                                     unsigned int featureIndex) :
  m_window(window),
  m_strControllerId(strControllerId),
  m_feature(feature),
  m_featureIndex(featureIndex),
  m_analogStickDirection(CARDINAL_DIRECTION::UNKNOWN),
  m_bAborted(false)
{
}

bool CGUIFeatureButton::PromptForInput(void)
{
  m_bAborted = false;
  m_waitCondition.Reset();

  switch (m_feature.Type())
  {
    case FEATURE_TYPE::SCALAR:
    {
      PromptButton();
      break;
    }
    case FEATURE_TYPE::ANALOG_STICK:
    {
      PromptAnalogStick();
      break;
    }
    default:
      break;
  }

  return !m_bAborted;
}

void CGUIFeatureButton::PromptButton(void)
{
  std::string strPromptTemplate = g_localizeStrings.Get(35051); // "Press %s..."
  std::string strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
  SetLabel(strPrompt);

  m_waitCondition.Wait();

  SetLabel(m_feature.Label());
}

void CGUIFeatureButton::PromptAnalogStick(void)
{
  m_analogStickDirection = CARDINAL_DIRECTION::UP;

  while (m_analogStickDirection != CARDINAL_DIRECTION::UNKNOWN)
  {
    // Process direection
    switch (m_analogStickDirection)
    {
      case CARDINAL_DIRECTION::UP:
      {
        std::string strPromptTemplate = g_localizeStrings.Get(35052); // "Move %s up..."
        std::string strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
        SetLabel(strPrompt);
        break;
      }
      case CARDINAL_DIRECTION::DOWN:
      {
        std::string strPromptTemplate = g_localizeStrings.Get(35053); // "Move %s down..."
        std::string strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
        SetLabel(strPrompt);
        break;
      }
      case CARDINAL_DIRECTION::RIGHT:
      {
        std::string strPromptTemplate = g_localizeStrings.Get(35054); // "Move %s right..."
        std::string strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
        SetLabel(strPrompt);
        break;
      }
      case CARDINAL_DIRECTION::LEFT:
      {
        std::string strPromptTemplate = g_localizeStrings.Get(35055); // "Move %s left..."
        std::string strPrompt = StringUtils::Format(strPromptTemplate.c_str(), m_feature.Label().c_str());
        SetLabel(strPrompt);
        break;
      }
      default:
        break;
    }

    m_waitCondition.Wait();

    if (m_bAborted)
      break;

    // Update direction
    switch (m_analogStickDirection)
    {
      case CARDINAL_DIRECTION::UP:
      {
        m_analogStickDirection = CARDINAL_DIRECTION::RIGHT;
        break;
      }
      case CARDINAL_DIRECTION::RIGHT:
      {
        m_analogStickDirection = CARDINAL_DIRECTION::DOWN;
        break;
      }
      case CARDINAL_DIRECTION::DOWN:
      {
        m_analogStickDirection = CARDINAL_DIRECTION::LEFT;
        break;
      }
      case CARDINAL_DIRECTION::LEFT:
      default:
      {
        m_analogStickDirection = CARDINAL_DIRECTION::UNKNOWN;
        break;
      }
    }
  }

  SetLabel(m_feature.Label());
}

void CGUIFeatureButton::Abort(void)
{
  m_bAborted = true;
  m_waitCondition.Set();
}

bool CGUIFeatureButton::MapPrimitive(IJoystickButtonMap* buttonMap, const CDriverPrimitive& primitive)
{
  bool bHandled = false;

  // Handle esc key separately
  if (primitive.Type() == CDriverPrimitive::BUTTON &&
      primitive.Index() == ESC_KEY_CODE)
  {
    bHandled = true;
    Abort();
  }
  else
  {
    switch (m_feature.Type())
    {
      case FEATURE_TYPE::SCALAR:
      {
        bHandled = buttonMap->AddScalar(m_feature.Name(), primitive);
        break;
      }
      case FEATURE_TYPE::ANALOG_STICK:
      {
        CDriverPrimitive up;
        CDriverPrimitive down;
        CDriverPrimitive right;
        CDriverPrimitive left;

        buttonMap->GetAnalogStick(m_feature.Name(), up, down, right, left);

        switch (m_analogStickDirection)
        {
          case CARDINAL_DIRECTION::UP:    up    = primitive; break;
          case CARDINAL_DIRECTION::DOWN:  down  = primitive; break;
          case CARDINAL_DIRECTION::RIGHT: right = primitive; break;
          case CARDINAL_DIRECTION::LEFT:  left  = primitive; break;
          default:
            break;
        }

        bHandled = buttonMap->AddAnalogStick(m_feature.Name(), up, down, right, left);

        break;
      }
      default:
        break;
    }

    if (bHandled)
    {
      m_lastPrimtive = primitive;
      m_waitCondition.Set();
    }
  }

  return bHandled;
}

void CGUIFeatureButton::SetLabel(const std::string& strPromptMsg)
{
  m_window->SetLabel(m_featureIndex, strPromptMsg);
}
