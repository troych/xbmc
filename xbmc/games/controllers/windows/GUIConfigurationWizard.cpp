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

#include "GUIConfigurationWizard.h"
#include "games/controllers/guicontrols/GUIFeatureButton.h"
#include "games/controllers/Controller.h"
#include "games/controllers/ControllerFeature.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "peripherals/Peripherals.h"
#include "threads/SingleLock.h"
#include "utils/log.h"

using namespace GAME;

#define ESC_KEY_CODE  27

CGUIConfigurationWizard::CGUIConfigurationWizard(void) :
  CThread("GUIConfigurationWizard"),
  m_currentButton(nullptr),
  m_currentDirection(JOYSTICK::CARDINAL_DIRECTION::UNKNOWN)
{
}

void CGUIConfigurationWizard::Run(const std::string& strControllerId, const std::vector<IFeatureButton*>& buttons)
{
  Abort();

  m_strControllerId = strControllerId;
  m_buttons = buttons;
  m_currentButton = nullptr;
  m_history.clear();

  Create();
}

bool CGUIConfigurationWizard::Abort(bool bWait /* = true */)
{
  if (IsRunning())
  {
    StopThread(false);

    m_inputEvent.Set();

    if (bWait)
      StopThread(true);

    return true;
  }
  return false;
}

void CGUIConfigurationWizard::Process(void)
{
  CLog::Log(LOGDEBUG, "Starting configuration wizard");

  InstallHooks();

  for (IFeatureButton* button : m_buttons)
  {
    m_currentButton = button;

    while (!button->IsFinished())
    {
      m_currentDirection = button->GetDirection();

      if (!button->PromptForInput(m_inputEvent))
        Abort(false);

      if (m_bStop)
        break;
    }

    button->Reset();

    if (m_bStop)
      break;
  }

  m_currentButton = nullptr;

  RemoveHooks();

  CLog::Log(LOGDEBUG, "Configuration wizard ended");
}

bool CGUIConfigurationWizard::MapPrimitive(JOYSTICK::IJoystickButtonMap* buttonMap, const JOYSTICK::CDriverPrimitive& primitive)
{
  using namespace JOYSTICK;

  bool bHandled = false;

  // Handle esc key separately
  if (primitive.Type() == CDriverPrimitive::BUTTON &&
      primitive.Index() == ESC_KEY_CODE)
  {
    bHandled = Abort(false);
  }
  else if (m_history.find(primitive) != m_history.end())
  {
    // Primitive has already been mapped this round, ignore it
    bHandled = true;
  }
  else
  {
    IFeatureButton* currentButton = m_currentButton;
    if (currentButton)
    {
      const CControllerFeature& feature = currentButton->Feature();
      switch (feature.Type())
      {
        case FEATURE_TYPE::SCALAR:
        {
          bHandled = buttonMap->AddScalar(feature.Name(), primitive);
          break;
        }
        case FEATURE_TYPE::ANALOG_STICK:
        {
          CDriverPrimitive up;
          CDriverPrimitive down;
          CDriverPrimitive right;
          CDriverPrimitive left;

          buttonMap->GetAnalogStick(feature.Name(), up, down, right, left);

          switch (m_currentDirection)
          {
            case CARDINAL_DIRECTION::UP:    up    = primitive; break;
            case CARDINAL_DIRECTION::DOWN:  down  = primitive; break;
            case CARDINAL_DIRECTION::RIGHT: right = primitive; break;
            case CARDINAL_DIRECTION::LEFT:  left  = primitive; break;
            default:
              break;
          }

          bHandled = buttonMap->AddAnalogStick(feature.Name(), up, down, right, left);

          break;
        }
        default:
          break;
      }

      if (bHandled)
      {
        m_history.insert(primitive);
        m_inputEvent.Set();
      }
    }
  }
  
  return bHandled;
}

void CGUIConfigurationWizard::InstallHooks(void)
{
  using namespace PERIPHERALS;

  g_peripherals.RegisterJoystickButtonMapper(this);
  g_peripherals.RegisterObserver(this);
}

void CGUIConfigurationWizard::RemoveHooks(void)
{
  using namespace PERIPHERALS;

  g_peripherals.UnregisterObserver(this);
  g_peripherals.UnregisterJoystickButtonMapper(this);
}

void CGUIConfigurationWizard::Notify(const Observable& obs, const ObservableMessage msg)
{
  using namespace PERIPHERALS;

  switch (msg)
  {
    case ObservableMessagePeripheralsChanged:
    {
      g_peripherals.UnregisterJoystickButtonMapper(this);
      g_peripherals.RegisterJoystickButtonMapper(this);
      break;
    }
    default:
      break;
  }
}
