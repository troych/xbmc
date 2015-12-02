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

#include "AddonJoystickButtonMap.h"
#include "input/joysticks/JoystickUtils.h"
#include "peripherals/Peripherals.h"
#include "peripherals/devices/Peripheral.h"
#include "utils/log.h"

using namespace JOYSTICK;
using namespace PERIPHERALS;

CAddonJoystickButtonMap::CAddonJoystickButtonMap(CPeripheral* device, const std::string& strControllerId)
  : m_device(device),
    m_addon(g_peripherals.GetAddon(device)),
    m_strControllerId(strControllerId)
{
  if (m_addon)
    m_addon->RegisterButtonMap(device, this);
  else
    CLog::Log(LOGDEBUG, "Failed to locate add-on for device \"%s\"", device->DeviceName().c_str());
}

CAddonJoystickButtonMap::~CAddonJoystickButtonMap(void)
{
  if (m_addon)
    m_addon->UnregisterButtonMap(this);
}

bool CAddonJoystickButtonMap::Load(void)
{
  if (m_addon)
  {
    m_features.clear();
    m_driverMap.clear();

    if (m_addon->GetFeatures(m_device, m_strControllerId, m_features))
    {
      CLog::Log(LOGDEBUG, "Loaded button map with %lu features for controller %s",
                m_features.size(), m_strControllerId.c_str());

      m_driverMap = CreateLookupTable(m_features);

      return true;
    }

    CLog::Log(LOGDEBUG, "Failed to load button map for device \"%s\"", m_device->DeviceName().c_str());
  }
  return false;
}

bool CAddonJoystickButtonMap::GetFeature(const CDriverPrimitive& primitive, FeatureName& feature)
{
  DriverMap::const_iterator it = m_driverMap.find(primitive);
  if (it != m_driverMap.end())
  {
    feature = it->second;
    return true;
  }

  return false;
}

bool CAddonJoystickButtonMap::GetScalar(const FeatureName& feature, CDriverPrimitive& primitive)
{
  bool retVal(false);

  FeatureMap::const_iterator it = m_features.find(feature);
  if (it != m_features.end())
  {
    const ADDON::JoystickFeature& addonFeature = it->second;

    if (addonFeature.Type() == JOYSTICK_FEATURE_TYPE_SCALAR)
    {
      primitive = ToPrimitive(addonFeature.Primitive());
      retVal = true;
    }
  }

  return retVal;
}

bool CAddonJoystickButtonMap::AddScalar(const FeatureName& feature, const CDriverPrimitive& primitive)
{
  ADDON::JoystickFeature scalar(feature, JOYSTICK_FEATURE_TYPE_SCALAR);
  scalar.SetPrimitive(ToPrimitive(primitive));

  return m_addon->AddFeature(m_device, m_strControllerId, scalar);
}

bool CAddonJoystickButtonMap::GetAnalogStick(const FeatureName& feature,
                                             CDriverPrimitive& up,
                                             CDriverPrimitive& down,
                                             CDriverPrimitive& right,
                                             CDriverPrimitive& left)
{
  bool retVal(false);

  FeatureMap::const_iterator it = m_features.find(feature);
  if (it != m_features.end())
  {
    const ADDON::JoystickFeature& addonFeature = it->second;

    if (addonFeature.Type() == JOYSTICK_FEATURE_TYPE_ANALOG_STICK)
    {
      up     = ToPrimitive(addonFeature.Up());
      down   = ToPrimitive(addonFeature.Down());
      right  = ToPrimitive(addonFeature.Right());
      left   = ToPrimitive(addonFeature.Left());
      retVal = true;
    }
  }

  return retVal;
}

bool CAddonJoystickButtonMap::AddAnalogStick(const FeatureName& feature,
                                             const CDriverPrimitive& up,
                                             const CDriverPrimitive& down,
                                             const CDriverPrimitive& right,
                                             const CDriverPrimitive& left)
{
  ADDON::JoystickFeature analogStick(feature, JOYSTICK_FEATURE_TYPE_ANALOG_STICK);

  analogStick.SetUp(ToPrimitive(up));
  analogStick.SetDown(ToPrimitive(down));
  analogStick.SetRight(ToPrimitive(right));
  analogStick.SetLeft(ToPrimitive(left));

  return m_addon->AddFeature(m_device, m_strControllerId, analogStick);
}

bool CAddonJoystickButtonMap::GetAccelerometer(const FeatureName& feature,
                                               CDriverPrimitive& positiveX,
                                               CDriverPrimitive& positiveY,
                                               CDriverPrimitive& positiveZ)
{
  bool retVal(false);

  FeatureMap::const_iterator it = m_features.find(feature);
  if (it != m_features.end())
  {
    const ADDON::JoystickFeature& addonFeature = it->second;

    if (addonFeature.Type() == JOYSTICK_FEATURE_TYPE_ACCELEROMETER)
    {
      positiveX = ToPrimitive(addonFeature.PositiveX());
      positiveY = ToPrimitive(addonFeature.PositiveY());
      positiveZ = ToPrimitive(addonFeature.PositiveZ());
      retVal    = true;
    }
  }

  return retVal;
}

bool CAddonJoystickButtonMap::AddAccelerometer(const FeatureName& feature,
                                               const CDriverPrimitive& positiveX,
                                               const CDriverPrimitive& positiveY,
                                               const CDriverPrimitive& positiveZ)
{
  ADDON::JoystickFeature accelerometer(feature, JOYSTICK_FEATURE_TYPE_ACCELEROMETER);

  accelerometer.SetPositiveX(ToPrimitive(positiveX));
  accelerometer.SetPositiveY(ToPrimitive(positiveY));
  accelerometer.SetPositiveZ(ToPrimitive(positiveZ));

  return m_addon->AddFeature(m_device, m_strControllerId, accelerometer);
}

CAddonJoystickButtonMap::DriverMap CAddonJoystickButtonMap::CreateLookupTable(const FeatureMap& features)
{
  DriverMap driverMap;

  for (FeatureMap::const_iterator it = features.begin(); it != features.end(); ++it)
  {
    const ADDON::JoystickFeature& feature = it->second;

    switch (feature.Type())
    {
      case JOYSTICK_FEATURE_TYPE_SCALAR:
      {
        driverMap[ToPrimitive(feature.Primitive())] = it->first;
        break;
      }

      case JOYSTICK_FEATURE_TYPE_ANALOG_STICK:
      {
        driverMap[ToPrimitive(feature.Up())] = it->first;
        driverMap[ToPrimitive(feature.Down())] = it->first;
        driverMap[ToPrimitive(feature.Right())] = it->first;
        driverMap[ToPrimitive(feature.Left())] = it->first;
        break;
      }

      case JOYSTICK_FEATURE_TYPE_ACCELEROMETER:
      {
        CDriverPrimitive x_axis(ToPrimitive(feature.PositiveX()));
        CDriverPrimitive y_axis(ToPrimitive(feature.PositiveY()));
        CDriverPrimitive z_axis(ToPrimitive(feature.PositiveZ()));

        driverMap[x_axis] = it->first;
        driverMap[y_axis] = it->first;
        driverMap[z_axis] = it->first;

        CDriverPrimitive x_axis_opposite(x_axis.Index(), x_axis.SemiAxisDirection() * -1);
        CDriverPrimitive y_axis_opposite(y_axis.Index(), y_axis.SemiAxisDirection() * -1);
        CDriverPrimitive z_axis_opposite(z_axis.Index(), z_axis.SemiAxisDirection() * -1);

        driverMap[x_axis_opposite] = it->first;
        driverMap[y_axis_opposite] = it->first;
        driverMap[z_axis_opposite] = it->first;
        break;
      }
        
      default:
        break;
    }
  }
  
  return driverMap;
}

CDriverPrimitive CAddonJoystickButtonMap::ToPrimitive(const ADDON::DriverPrimitive& primitive)
{
  CDriverPrimitive retVal;

  switch (primitive.Type())
  {
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
    {
      retVal = CDriverPrimitive(primitive.DriverIndex());
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
    {
      retVal = CDriverPrimitive(primitive.DriverIndex(), ToHatDirection(primitive.HatDirection()));
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
    {
      retVal = CDriverPrimitive(primitive.DriverIndex(), ToSemiAxisDirection(primitive.SemiAxisDirection()));
      break;
    }
    default:
      break;
  }

  return retVal;
}

ADDON::DriverPrimitive CAddonJoystickButtonMap::ToPrimitive(const CDriverPrimitive& primitive)
{
  ADDON::DriverPrimitive retVal;

  switch (primitive.Type())
  {
    case CDriverPrimitive::Button:
    {
      retVal = ADDON::DriverPrimitive(primitive.Index());
      break;
    }
    case CDriverPrimitive::Hat:
    {
      retVal = ADDON::DriverPrimitive(primitive.Index(), ToHatDirection(primitive.HatDirection()));
      break;
    }
    case CDriverPrimitive::SemiAxis:
    {
      retVal = ADDON::DriverPrimitive(primitive.Index(), ToSemiAxisDirection(primitive.SemiAxisDirection()));
      break;
    }
    default:
      break;
  }

  return retVal;
}

HAT_DIRECTION CAddonJoystickButtonMap::ToHatDirection(JOYSTICK_DRIVER_HAT_DIRECTION driverDirection)
{
  switch (driverDirection)
  {
    case JOYSTICK_DRIVER_HAT_LEFT:   return HAT_DIRECTION::LEFT;
    case JOYSTICK_DRIVER_HAT_RIGHT:  return HAT_DIRECTION::RIGHT;
    case JOYSTICK_DRIVER_HAT_UP:     return HAT_DIRECTION::UP;
    case JOYSTICK_DRIVER_HAT_DOWN:   return HAT_DIRECTION::DOWN;
    default:
      break;
  }
  return HAT_DIRECTION::UNKNOWN;
}

JOYSTICK_DRIVER_HAT_DIRECTION CAddonJoystickButtonMap::ToHatDirection(HAT_DIRECTION dir)
{
  switch (dir)
  {
    case HAT_DIRECTION::UP:     return JOYSTICK_DRIVER_HAT_UP;
    case HAT_DIRECTION::DOWN:   return JOYSTICK_DRIVER_HAT_DOWN;
    case HAT_DIRECTION::RIGHT:  return JOYSTICK_DRIVER_HAT_RIGHT;
    case HAT_DIRECTION::LEFT:   return JOYSTICK_DRIVER_HAT_LEFT;
    default:
      break;
  }
  return JOYSTICK_DRIVER_HAT_UNKNOWN;
}

SEMIAXIS_DIRECTION CAddonJoystickButtonMap::ToSemiAxisDirection(JOYSTICK_DRIVER_SEMIAXIS_DIRECTION dir)
{
  switch (dir)
  {
    case JOYSTICK_DRIVER_SEMIAXIS_POSITIVE: return SEMIAXIS_DIRECTION::POSITIVE;
    case JOYSTICK_DRIVER_SEMIAXIS_NEGATIVE: return SEMIAXIS_DIRECTION::NEGATIVE;
    default:
      break;
  }
  return SEMIAXIS_DIRECTION::UNKNOWN;
}

JOYSTICK_DRIVER_SEMIAXIS_DIRECTION CAddonJoystickButtonMap::ToSemiAxisDirection(SEMIAXIS_DIRECTION dir)
{
  switch (dir)
  {
    case SEMIAXIS_DIRECTION::POSITIVE: return JOYSTICK_DRIVER_SEMIAXIS_POSITIVE;
    case SEMIAXIS_DIRECTION::NEGATIVE: return JOYSTICK_DRIVER_SEMIAXIS_NEGATIVE;
    default:
      break;
  }
  return JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN;
}
