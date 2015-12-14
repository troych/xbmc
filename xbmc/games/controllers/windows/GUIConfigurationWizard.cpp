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
#include "games/controllers/Controller.h"
#include "peripherals/Peripherals.h"

using namespace GAME;
using namespace PERIPHERALS;

CGUIConfigurationWizard::CGUIConfigurationWizard(IFeatureList* featureList) :
  CThread("GUIConfigurationWizard"),
  m_features(featureList),
  m_featureIndex(0),
  m_bAborted(false)
{
}

void CGUIConfigurationWizard::Run(unsigned int featureIndex)
{
  if (IsRunning())
    Abort();

  m_featureIndex = featureIndex;

  Create();
}

bool CGUIConfigurationWizard::Abort(void)
{
  if (IsRunning())
  {
    m_bAborted = true;
    m_features->AbortPrompt();
    StopThread(true);
    return true;
  }
  return false;
}

void CGUIConfigurationWizard::Process(void)
{
  InstallHooks();
  for (m_bAborted = false; !m_bAborted; m_featureIndex++)
  {
    if (!m_features->PromptForInput(m_featureIndex))
      m_bAborted = true;
  }
  RemoveHooks();
}

std::string CGUIConfigurationWizard::ControllerID(void) const
{
  ControllerPtr controller = m_features->GetActiveController();
  if (controller)
    return controller->ID();

  return "";
}

bool CGUIConfigurationWizard::MapPrimitive(JOYSTICK::IJoystickButtonMap* buttonMap, const JOYSTICK::CDriverPrimitive& primitive)
{
  JOYSTICK::IJoystickButtonMapper* buttonMapper = m_features->GetButtonMapper();
  if (buttonMapper)
    return buttonMapper->MapPrimitive(buttonMap, primitive);

  return false;
}

void CGUIConfigurationWizard::InstallHooks(void)
{
  g_peripherals.RegisterJoystickButtonMapper(this);
  g_peripherals.RegisterObserver(this);
}

void CGUIConfigurationWizard::RemoveHooks(void)
{
  g_peripherals.UnregisterObserver(this);
  g_peripherals.UnregisterJoystickButtonMapper(this);
}

void CGUIConfigurationWizard::Notify(const Observable& obs, const ObservableMessage msg)
{
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
