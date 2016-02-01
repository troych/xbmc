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

#include "GenericKeyboardJoystickHandling.h"
#include "input/joysticks/IJoystickDriverHandler.h"
#include "input/Key.h"

#include <assert.h>

#define BUTTON_INDEX_MASK  0x01ff

using namespace KEYBOARD;

CGenericKeyboardJoystickHandling::CGenericKeyboardJoystickHandling(JOYSTICK::IJoystickDriverHandler* handler) :
  m_handler(handler)
{
  assert(m_handler);
}

bool CGenericKeyboardJoystickHandling::OnKeyPress(const CKey& key)
{
  bool bHandled = false;

  unsigned int buttonIndex = GetButtonIndex(key);
  if (buttonIndex != 0)
    bHandled = m_handler->OnButtonMotion(buttonIndex, true);

  return bHandled;
}

void CGenericKeyboardJoystickHandling::OnKeyRelease(const CKey& key)
{
  unsigned int buttonIndex = GetButtonIndex(key);
  if (buttonIndex != 0)
    m_handler->OnButtonMotion(buttonIndex, false);
}

unsigned int CGenericKeyboardJoystickHandling::GetButtonIndex(const CKey& key)
{
  return key.GetButtonCode() & BUTTON_INDEX_MASK;
}
