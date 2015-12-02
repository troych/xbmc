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

// --- Helper function ---------------------------------------------------------

JOYSTICK_DRIVER_SEMIAXIS_DIRECTION operator*(JOYSTICK_DRIVER_SEMIAXIS_DIRECTION dir, int i)
{
  return static_cast<JOYSTICK_DRIVER_SEMIAXIS_DIRECTION>(static_cast<int>(dir) * i);
}

// --- CAddonJoystickButtonMap -------------------------------------------------

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
  if (primitive.Type() == CDriverPrimitive::Unknown)
  {
    FeatureMap::iterator it = m_features.find(feature);
    if (it != m_features.end())
      m_features.erase(it);
  }
  else
  {
    UnmapPrimitive(primitive);

    ADDON::JoystickFeature scalar(feature, JOYSTICK_FEATURE_TYPE_SCALAR);
    scalar.SetPrimitive(ToPrimitive(primitive));

    m_features[feature] = scalar;
  }

  m_driverMap = CreateLookupTable(m_features);

  return m_addon->MapFeatures(m_device, m_strControllerId, m_features);
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
  if (up.Type() == CDriverPrimitive::Unknown &&
      down.Type() == CDriverPrimitive::Unknown &&
      right.Type() == CDriverPrimitive::Unknown &&
      left.Type() == CDriverPrimitive::Unknown)
  {
    FeatureMap::iterator it = m_features.find(feature);
    if (it != m_features.end())
      m_features.erase(it);
  }
  else
  {
    ADDON::JoystickFeature analogStick(feature, JOYSTICK_FEATURE_TYPE_ANALOG_STICK);

    if (up.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(up);
      analogStick.SetUp(ToPrimitive(up));
    }
    if (down.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(down);
      analogStick.SetDown(ToPrimitive(down));
    }
    if (right.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(right);
      analogStick.SetRight(ToPrimitive(right));
    }
    if (left.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(left);
      analogStick.SetLeft(ToPrimitive(left));
    }

    m_features[feature] = analogStick;
  }

  m_driverMap = CreateLookupTable(m_features);

  return m_addon->MapFeatures(m_device, m_strControllerId, m_features);
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
  if (positiveX.Type() == CDriverPrimitive::Unknown &&
      positiveY.Type() == CDriverPrimitive::Unknown &&
      positiveZ.Type() == CDriverPrimitive::Unknown)
  {
    FeatureMap::iterator it = m_features.find(feature);
    if (it != m_features.end())
      m_features.erase(it);
  }
  else
  {
    ADDON::JoystickFeature accelerometer(feature, JOYSTICK_FEATURE_TYPE_ACCELEROMETER);

    if (positiveX.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(positiveX);
      accelerometer.SetPositiveX(ToPrimitive(positiveX));
    }
    if (positiveY.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(positiveY);
      accelerometer.SetPositiveY(ToPrimitive(positiveY));
    }
    if (positiveZ.Type() != CDriverPrimitive::Unknown)
    {
      UnmapPrimitive(positiveZ);
      accelerometer.SetPositiveZ(ToPrimitive(positiveZ));
    }

    // TODO: Unmap complementary semiaxes

    m_features[feature] = accelerometer;
  }

  m_driverMap = CreateLookupTable(m_features);

  return m_addon->MapFeatures(m_device, m_strControllerId, m_features);
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

bool CAddonJoystickButtonMap::UnmapPrimitive(const CDriverPrimitive& primitive)
{
  bool bModified = false;

  DriverMap::iterator it = m_driverMap.find(primitive);
  if (it != m_driverMap.end())
  {
    const FeatureName& featureName = it->second;
    FeatureMap::iterator itFeature = m_features.find(featureName);
    if (itFeature != m_features.end())
    {
      ADDON::JoystickFeature& addonFeature = itFeature->second;
      ResetPrimitive(addonFeature, ToPrimitive(primitive));
      if (addonFeature.Type() == JOYSTICK_FEATURE_TYPE_UNKNOWN)
        m_features.erase(itFeature);
      bModified = true;
    }
  }

  return bModified;
}

bool CAddonJoystickButtonMap::ResetPrimitive(ADDON::JoystickFeature& feature, const ADDON::DriverPrimitive& primitive)
{
  bool bModified = false;

  switch (feature.Type())
  {
    case JOYSTICK_FEATURE_TYPE_SCALAR:
    {
      if (primitive == feature.Primitive())
      {
        CLog::Log(LOGDEBUG, "Removing \"%s\" from button map due to conflict", feature.Name().c_str());
        feature.SetType(JOYSTICK_FEATURE_TYPE_UNKNOWN);
        bModified = true;
      }
      break;
    }
    case JOYSTICK_FEATURE_TYPE_ANALOG_STICK:
    {
      if (primitive == feature.Up())
      {
        feature.SetUp(ADDON::DriverPrimitive());
        bModified = true;
      }
      else if (primitive == feature.Down())
      {
        feature.SetDown(ADDON::DriverPrimitive());
        bModified = true;
      }
      else if (primitive == feature.Right())
      {
        feature.SetRight(ADDON::DriverPrimitive());
        bModified = true;
      }
      else if (primitive == feature.Left())
      {
        feature.SetLeft(ADDON::DriverPrimitive());
        bModified = true;
      }

      if (bModified)
      {
        if (feature.Up().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
            feature.Down().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
            feature.Right().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
            feature.Left().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
        {
          CLog::Log(LOGDEBUG, "Removing \"%s\" from button map due to conflict", feature.Name().c_str());
          feature.SetType(JOYSTICK_FEATURE_TYPE_UNKNOWN);
        }
      }
      break;
    }
    case JOYSTICK_FEATURE_TYPE_ACCELEROMETER:
    {
      if (primitive == feature.PositiveX() ||
          primitive == Opposite(feature.PositiveX()))
      {
        feature.SetPositiveX(ADDON::DriverPrimitive());
        bModified = true;
      }
      else if (primitive == feature.PositiveY() ||
               primitive == Opposite(feature.PositiveY()))
      {
        feature.SetPositiveY(ADDON::DriverPrimitive());
        bModified = true;
      }
      else if (primitive == feature.PositiveZ() ||
               primitive == Opposite(feature.PositiveZ()))
      {
        feature.SetPositiveZ(ADDON::DriverPrimitive());
        bModified = true;
      }

      if (bModified)
      {
        if (feature.PositiveX().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
            feature.PositiveY().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
            feature.PositiveZ().Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
        {
          CLog::Log(LOGDEBUG, "Removing \"%s\" from button map due to conflict", feature.Name().c_str());
          feature.SetType(JOYSTICK_FEATURE_TYPE_UNKNOWN);
        }
      }
      break;
    }
    default:
      break;
  }
  return bModified;
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

ADDON::DriverPrimitive CAddonJoystickButtonMap::Opposite(const ADDON::DriverPrimitive& primitive)
{
  return ADDON::DriverPrimitive(primitive.DriverIndex(), primitive.SemiAxisDirection() * -1);
}
