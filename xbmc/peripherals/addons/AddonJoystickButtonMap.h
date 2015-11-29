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

#include "AddonJoystickButtonMapRO.h"
#include "AddonJoystickButtonMapWO.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "peripherals/addons/PeripheralAddon.h"

namespace PERIPHERALS
{
  class CAddonJoystickButtonMap : public JOYSTICK::IJoystickButtonMap
  {
  public:
    CAddonJoystickButtonMap(CPeripheral* device, const std::string& strControllerId);

    virtual ~CAddonJoystickButtonMap(void);

    // Implementation of IJoystickButtonMap
    virtual std::string ControllerID(void) const override { return m_buttonMapRO.ControllerID(); }

    virtual bool Load(void) override;

    virtual bool GetFeature(
      const JOYSTICK::CDriverPrimitive& primitive,
      JOYSTICK::JoystickFeature& feature
    ) override;

    virtual bool GetScalar(
      const JOYSTICK::JoystickFeature& feature,
      JOYSTICK::CDriverPrimitive& primitive
    ) override;

    virtual bool AddScalar(
      const JOYSTICK::JoystickFeature& feature,
      const JOYSTICK::CDriverPrimitive& primitive
    ) override;

    virtual bool GetAnalogStick(
      const JOYSTICK::JoystickFeature& feature,
      JOYSTICK::CDriverPrimitive& up,
      JOYSTICK::CDriverPrimitive& down,
      JOYSTICK::CDriverPrimitive& right,
      JOYSTICK::CDriverPrimitive& left
    ) override;

    virtual bool AddAnalogStick(
      const JOYSTICK::JoystickFeature& feature,
      const JOYSTICK::CDriverPrimitive& up,
      const JOYSTICK::CDriverPrimitive& down,
      const JOYSTICK::CDriverPrimitive& right,
      const JOYSTICK::CDriverPrimitive& left
    ) override;

    virtual bool GetAccelerometer(
      const JOYSTICK::JoystickFeature& feature,
      JOYSTICK::CDriverPrimitive& positiveX,
      JOYSTICK::CDriverPrimitive& positiveY,
      JOYSTICK::CDriverPrimitive& positiveZ
    ) override;

    virtual bool AddAccelerometer(
      const JOYSTICK::JoystickFeature& feature,
      const JOYSTICK::CDriverPrimitive& positiveX,
      const JOYSTICK::CDriverPrimitive& positiveY,
      const JOYSTICK::CDriverPrimitive& positiveZ
    ) override;

  private:
    PeripheralAddonPtr        m_addon;
    CAddonJoystickButtonMapRO m_buttonMapRO;
    CAddonJoystickButtonMapWO m_buttonMapWO;
  };
}
