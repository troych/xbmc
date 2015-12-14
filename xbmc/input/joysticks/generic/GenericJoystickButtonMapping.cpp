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
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMapper.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/joysticks/JoystickUtils.h"
#include "threads/SystemClock.h"

#include <algorithm>
#include <assert.h>
#include <cmath>

using namespace JOYSTICK;
using namespace XbmcThreads;

#define AXIS_THRESHOLD    0.5f
#define AXIS_HOLDTIME_MS  100

CGenericJoystickButtonMapping::CGenericJoystickButtonMapping(IJoystickButtonMapper* buttonMapper, IJoystickButtonMap* buttonMap)
  : m_buttonMapper(buttonMapper),
    m_buttonMap(buttonMap)
{
  assert(m_buttonMapper != NULL);
  assert(m_buttonMap != NULL);
}

bool CGenericJoystickButtonMapping::OnButtonMotion(unsigned int buttonIndex, bool bPressed)
{
  if (bPressed)
  {
    CDriverPrimitive buttonPrimitive(buttonIndex);
    m_buttonMapper->MapPrimitive(m_buttonMap, buttonPrimitive);
  }

  return true;
}

bool CGenericJoystickButtonMapping::OnHatMotion(unsigned int hatIndex, HAT_STATE state)
{
  CDriverPrimitive hatPrimitive(hatIndex, static_cast<HAT_DIRECTION>(state));
  if (hatPrimitive.IsValid())
    m_buttonMapper->MapPrimitive(m_buttonMap, hatPrimitive);

  return true;
}

bool CGenericJoystickButtonMapping::OnAxisMotion(unsigned int axisIndex, float position)
{
  SEMIAXIS_DIRECTION dir = CJoystickTranslator::PositionToSemiAxisDirection(position);

  CDriverPrimitive axis(axisIndex, dir);
  CDriverPrimitive oppositeAxis(axisIndex, dir * -1);

  if (position == 0.0f)
  {
    Deactivate(axis);
    Deactivate(oppositeAxis);
  }
  else
  {
    Deactivate(oppositeAxis);

    if (std::abs(position) >= AXIS_THRESHOLD)
      Activate(axis);
    else
      Deactivate(axis);
  }

  return true;
}

void CGenericJoystickButtonMapping::ProcessAxisMotions(void)
{
  for (std::vector<ActivatedAxis>::iterator it = m_activatedAxes.begin(); it != m_activatedAxes.end(); ++it)
  {
    ActivatedAxis& semiaxis = *it;

    // Only emit once
    if (!semiaxis.bEmitted)
    {
      bool bHeld = (SystemClockMillis() >= semiaxis.timestamp + AXIS_HOLDTIME_MS);

      if (bHeld)
      {
        semiaxis.bEmitted = true;
        m_buttonMapper->MapPrimitive(m_buttonMap, semiaxis.driverPrimitive);
      }
    }
  }
}

void CGenericJoystickButtonMapping::Activate(const CDriverPrimitive& semiaxis)
{
  if (!IsActive(semiaxis))
    m_activatedAxes.push_back(ActivatedAxis{SystemClockMillis(), semiaxis});
}

void CGenericJoystickButtonMapping::Deactivate(const CDriverPrimitive& semiaxis)
{
  m_activatedAxes.erase(std::remove_if(m_activatedAxes.begin(), m_activatedAxes.end(),
    [&semiaxis](const ActivatedAxis& axis)
    {
      return semiaxis == axis.driverPrimitive;
    }), m_activatedAxes.end());
}

bool CGenericJoystickButtonMapping::IsActive(const CDriverPrimitive& semiaxis)
{
  return std::find_if(m_activatedAxes.begin(), m_activatedAxes.end(),
    [&semiaxis](const ActivatedAxis& axis)
    {
      return semiaxis == axis.driverPrimitive;
    }) != m_activatedAxes.end();
}
