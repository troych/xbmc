/*
 *      Copyright (C) 2015-2016 Team Kodi
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

#include "PeripheralKeyboard.h"
#include "input/keyboard/generic/GenericKeyboardJoystick.h"
#include "input/InputManager.h"

using namespace PERIPHERALS;

CPeripheralKeyboard::CPeripheralKeyboard(const PeripheralScanResult& scanResult) :
  CPeripheral(scanResult),
  m_keyboardHandler(nullptr)
{
  m_features.push_back(FEATURE_KEYBOARD);
}

CPeripheralKeyboard::~CPeripheralKeyboard(void)
{
  if (m_keyboardHandler)
  {
    CInputManager::Get().UnregisterKeyboardHandler(m_keyboardHandler);
    delete m_keyboardHandler;
  }
}

bool CPeripheralKeyboard::InitialiseFeature(const PeripheralFeature feature)
{
  bool bSuccess = false;

  if (CPeripheral::InitialiseFeature(feature))
  {
    if (feature == FEATURE_KEYBOARD)
    {
      m_keyboardHandler = new KEYBOARD::CGenericKeyboardJoystick;
      CInputManager::Get().RegisterKeyboardHandler(m_keyboardHandler);
    }
    bSuccess = true;
  }

  return bSuccess;
}

void CPeripheralKeyboard::RegisterJoystickDriverHandler(JOYSTICK::IJoystickDriverHandler* handler, bool bPromiscuous)
{
  m_keyboardHandler->RegisterJoystickDriverHandler(handler, bPromiscuous);
}

void CPeripheralKeyboard::UnregisterJoystickDriverHandler(JOYSTICK::IJoystickDriverHandler* handler)
{
  m_keyboardHandler->UnregisterJoystickDriverHandler(handler);
}
