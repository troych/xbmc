/*
 *      Copyright (C) 2016 Team Kodi
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
#pragma once

namespace PERIPHERALS
{
  class CPeripheral;
}

namespace JOYSTICK
{
  class IButtonMap;

  /*!
   * \brief Analog axis deadzone filtering
   *
   * Axis is scaled appropriately, so position is continuous
   * from -1.0 to 1.0:
   *
   *            |    / 1.0
   *            |   /
   *          __|__/
   *         /  |
   *        /   |--| Deadzone
   *  -1.0 /    |
   */
  class CDeadzoneFilter
  {
  public:
    CDeadzoneFilter(IButtonMap* buttonMap, PERIPHERALS::CPeripheral* peripheral);

    float FilterAxis(unsigned int axisIndex, float axisValue);

  private:
    bool GetDeadzone(unsigned int axisIndex, float& result, const char* featureName, const char* settingName);

    static float ScaleDeadzone(float value, float deadzone);

    // Construction parameters
    IButtonMap* const               m_buttonMap;
    PERIPHERALS::CPeripheral* const m_peripheral;
  };
}
