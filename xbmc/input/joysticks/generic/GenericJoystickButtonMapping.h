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
#pragma once

#include "input/joysticks/IJoystickDriverHandler.h"

#include <vector>

namespace JOYSTICK
{
  class IJoystickButtonMap;
  class IJoystickButtonMapper;

  /*
   * \brief Generic implementation of a class that provides button mapping by
   *        translating driver events to button mapping commands
   */
  class CGenericJoystickButtonMapping : public IJoystickDriverHandler
  {
  public:
    /*
     * \brief Constructor for CGenericJoystickButtonMapping
     *
     * \param buttonMapper Carries out button-mapping commands using <buttonMap>
     * \param buttonMap The button map given to <buttonMapper> on each command
     */
    CGenericJoystickButtonMapping(IJoystickButtonMapper* buttonMapper, IJoystickButtonMap* buttonMap);

    virtual ~CGenericJoystickButtonMapping(void) { }

    // implementation of IJoystickDriverHandler
    virtual bool OnButtonMotion(unsigned int buttonIndex, bool bPressed) override;
    virtual bool OnHatMotion(unsigned int hatIndex, HAT_STATE state) override;
    virtual bool OnAxisMotion(unsigned int axisIndex, float position) override;
    virtual void ProcessAxisMotions(void) { }

  private:
    IJoystickButtonMapper* const m_buttonMapper;
    IJoystickButtonMap* const    m_buttonMap;
  };
}
