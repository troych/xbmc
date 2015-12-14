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

#include "GenericJoystickButtonMapping.h"
#include "input/joysticks/IJoystickButtonMapper.h"
#include "input/joysticks/JoystickTranslator.h"

#include <assert.h>

using namespace JOYSTICK;

CGenericJoystickButtonMapping::CGenericJoystickButtonMapping(IJoystickButtonMapper* buttonMapper, IJoystickButtonMap* buttonMap)
  : m_buttonMapper(buttonMapper),
    m_buttonMap(buttonMap)
{
  assert(m_buttonMapper != NULL);
  assert(m_buttonMap != NULL);
}

bool CGenericJoystickButtonMapping::OnButtonMotion(unsigned int buttonIndex, bool bPressed)
{
  return false;
}

bool CGenericJoystickButtonMapping::OnHatMotion(unsigned int hatIndex, HAT_STATE state)
{
  return false;
}

bool CGenericJoystickButtonMapping::OnAxisMotion(unsigned int axisIndex, float position)
{
  return false;
}
