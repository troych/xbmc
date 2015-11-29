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

#include "addons/include/kodi_peripheral_types.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/JoystickTypes.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "peripherals/addons/PeripheralAddon.h"

#include <string>

namespace PERIPHERALS
{
  class CAddonJoystickButtonMapWO : public JOYSTICK::IJoystickButtonMap
  {
  public:
    CAddonJoystickButtonMapWO(CPeripheral* device, const PeripheralAddonPtr& addon, const std::string& strControllerId);

    virtual ~CAddonJoystickButtonMapWO(void) { }

    // implementation of IJoystickButtonMap
    virtual std::string ControllerID(void) const override { return m_strControllerId; }

    virtual bool Load(void) override;

    virtual bool GetFeature(
      const JOYSTICK::CDriverPrimitive& primitive,
      JOYSTICK::JoystickFeature& feature
    ) override { return false; }

    virtual bool GetPrimitiveFeature(
      const JOYSTICK::JoystickFeature& feature,
      JOYSTICK::CDriverPrimitive& primitive
    ) override { return false; }

    virtual bool AddPrimitiveFeature(
      const JOYSTICK::JoystickFeature& feature,
      const JOYSTICK::CDriverPrimitive& primitive
    ) override;

    virtual bool GetAnalogStick(
      const JOYSTICK::JoystickFeature& feature,
      JOYSTICK::CDriverPrimitive& up,
      JOYSTICK::CDriverPrimitive& down,
      JOYSTICK::CDriverPrimitive& right,
      JOYSTICK::CDriverPrimitive& left
    ) override { return false; }

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
    ) override { return false; }

    virtual bool AddAccelerometer(
      const JOYSTICK::JoystickFeature& feature,
      const JOYSTICK::CDriverPrimitive& positiveX,
      const JOYSTICK::CDriverPrimitive& positiveY,
      const JOYSTICK::CDriverPrimitive& positiveZ
    ) override;

  private:
    // Utility functions
    static ADDON::DriverPrimitive             ToPrimitive(const JOYSTICK::CDriverPrimitive& primitive);
    static JOYSTICK_DRIVER_HAT_DIRECTION      ToHatDirection(JOYSTICK::HAT_DIRECTION dir);
    static JOYSTICK_DRIVER_SEMIAXIS_DIRECTION ToSemiAxisDirection(JOYSTICK::SEMIAXIS_DIRECTION dir);

    CPeripheral* const m_device;
    PeripheralAddonPtr m_addon;
    const std::string  m_strControllerId;
  };
}
