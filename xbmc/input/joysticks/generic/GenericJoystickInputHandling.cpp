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

#include "GenericJoystickInputHandling.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "input/joysticks/IJoystickInputHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/joysticks/JoystickUtils.h"
#include "utils/log.h"

#include <algorithm>

using namespace JOYSTICK;

#define ANALOG_DIGITAL_THRESHOLD  0.5f

// --- CJoystickFeature --------------------------------------------------------

CJoystickFeature::CJoystickFeature(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap) :
  m_name(name),
  m_handler(handler),
  m_buttonMap(buttonMap)
{
}

// --- CScalarFeature ----------------------------------------------------------

CScalarFeature::CScalarFeature(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap) :
  CJoystickFeature(name, handler, buttonMap),
  m_inputType(handler->GetInputType(name)),
  m_bDigitalState(false),
  m_analogState(0.0f)
{
}

bool CScalarFeature::OnDigitalMotion(const CDriverPrimitive& source, bool bPressed)
{
  if (m_inputType == INPUT_TYPE::DIGITAL)
  {
    if (m_bDigitalState != bPressed)
    {
      m_bDigitalState = bPressed;
      m_handler->OnButtonPress(m_name, bPressed);
    }
  }
  else if (m_inputType == INPUT_TYPE::ANALOG)
  {
    OnAnalogMotion(source, bPressed ? 1.0f : 0.0f);
  }

  return true;
}

bool CScalarFeature::OnAnalogMotion(const CDriverPrimitive& source, float magnitude)
{
  if (m_inputType == INPUT_TYPE::DIGITAL)
  {
    OnDigitalMotion(source, magnitude >= ANALOG_DIGITAL_THRESHOLD);
  }
  else if (m_inputType == INPUT_TYPE::ANALOG)
  {
    if (m_analogState != 0.0f || magnitude != 0.0f)
    {
      m_analogState = magnitude;
      m_handler->OnButtonMotion(m_name, magnitude);
    }
  }

  return true;
}

// --- CFeatureAxis ------------------------------------------------------------

CFeatureAxis::CFeatureAxis(void) :
  m_positiveDistance(0.0f),
  m_negativeDistance(0.0f)
{
}

float CFeatureAxis::GetPosition(void) const
{
  return m_positiveDistance - m_negativeDistance;
}

void CFeatureAxis::Reset(void)
{
  m_positiveDistance = 0.0f;
  m_negativeDistance = 0.0f;
}

// --- CAnalogStick ------------------------------------------------------------

CAnalogStick::CAnalogStick(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap) :
  CJoystickFeature(name, handler, buttonMap),
  m_vertState(0.0f),
  m_horizState(0.0f)
{
}

bool CAnalogStick::OnDigitalMotion(const CDriverPrimitive& source, bool bPressed)
{
  return OnAnalogMotion(source, bPressed ? 1.0f : 0.0f);
}

bool CAnalogStick::OnAnalogMotion(const CDriverPrimitive& source, float magnitude)
{
  CDriverPrimitive up;
  CDriverPrimitive down;
  CDriverPrimitive right;
  CDriverPrimitive left;

  m_buttonMap->GetAnalogStick(m_name, up, down, right,  left);

  if (source == up)
    m_vertAxis.SetPositiveDistance(magnitude);
  else if (source == down)
    m_vertAxis.SetNegativeDistance(magnitude);
  else if (source == right)
    m_horizAxis.SetPositiveDistance(magnitude);
  else if (source == left)
    m_horizAxis.SetNegativeDistance(magnitude);
  else
  {
    // Just in case, avoid sticking
    m_vertAxis.Reset();
    m_horizAxis.Reset();
  }

  return true;
}

void CAnalogStick::ProcessMotions(void)
{
  const float newVertState = m_vertAxis.GetPosition();
  const float newHorizState = m_horizAxis.GetPosition();

  if (m_vertState != 0 || m_horizState != 0 ||
      newVertState != 0 || newHorizState != 0)
  {
    m_vertState = newVertState;
    m_horizState = newHorizState;
    m_handler->OnAnalogStickMotion(m_name, newHorizState, newVertState);
  }
}

// --- CAccelerometer ----------------------------------------------------------

CAccelerometer::CAccelerometer(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap) :
  CJoystickFeature(name, handler, buttonMap),
  m_xAxisState(0.0f),
  m_yAxisState(0.0f),
  m_zAxisState(0.0f)
{
}

bool CAccelerometer::OnDigitalMotion(const CDriverPrimitive& source, bool bPressed)
{
  return OnAnalogMotion(source, bPressed ? 1.0f : 0.0f);
}

bool CAccelerometer::OnAnalogMotion(const CDriverPrimitive& source, float magnitude)
{
  CDriverPrimitive positiveX;
  CDriverPrimitive positiveY;
  CDriverPrimitive positiveZ;

  m_buttonMap->GetAccelerometer(m_name, positiveX, positiveY, positiveZ);

  if (source == positiveX)
    m_xAxis.SetPositiveDistance(magnitude);
  else if (source == positiveY)
    m_yAxis.SetPositiveDistance(magnitude);
  else if (source == positiveZ)
    m_zAxis.SetPositiveDistance(magnitude);
  else
  {
    // Just in case, avoid sticking
    m_xAxis.Reset();
    m_xAxis.Reset();
    m_yAxis.Reset();
  }

  return true;
}

void CAccelerometer::ProcessMotions(void)
{
  const float newXAxis = m_xAxis.GetPosition();
  const float newYAxis = m_yAxis.GetPosition();
  const float newZAxis = m_zAxis.GetPosition();

  if (m_xAxisState != 0 || m_yAxisState != 0 || m_zAxisState != 0 ||
      newXAxis != 0 || newYAxis != 0 || newZAxis)
  {
    m_xAxisState = newXAxis;
    m_yAxisState = newYAxis;
    m_zAxisState = newZAxis;
    m_handler->OnAccelerometerMotion(m_name, newXAxis, newYAxis, newZAxis);
  }
}

// --- CGenericJoystickInputHandling -------------------------------------------

CGenericJoystickInputHandling::CGenericJoystickInputHandling(IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap)
 : m_handler(handler),
   m_buttonMap(buttonMap)
{
}

CGenericJoystickInputHandling::~CGenericJoystickInputHandling(void)
{
}

bool CGenericJoystickInputHandling::OnButtonMotion(unsigned int buttonIndex, bool bPressed)
{
  return OnDigitalMotion(CDriverPrimitive(buttonIndex), bPressed);
}

bool CGenericJoystickInputHandling::OnHatMotion(unsigned int hatIndex, HAT_STATE state)
{
  bool bHandled = false;

  bHandled |= OnDigitalMotion(CDriverPrimitive(hatIndex, HAT_DIRECTION::UP),    state & HAT_DIRECTION::UP);
  bHandled |= OnDigitalMotion(CDriverPrimitive(hatIndex, HAT_DIRECTION::RIGHT), state & HAT_DIRECTION::RIGHT);
  bHandled |= OnDigitalMotion(CDriverPrimitive(hatIndex, HAT_DIRECTION::DOWN),  state & HAT_DIRECTION::DOWN);
  bHandled |= OnDigitalMotion(CDriverPrimitive(hatIndex, HAT_DIRECTION::LEFT),  state & HAT_DIRECTION::LEFT);

  return bHandled;
}

bool CGenericJoystickInputHandling::OnAxisMotion(unsigned int axisIndex, float position)
{
  bool bHandled = false;

  CDriverPrimitive positiveSemiaxis(axisIndex, SEMIAXIS_DIRECTION::POSITIVE);
  CDriverPrimitive negativeSemiaxis(axisIndex, SEMIAXIS_DIRECTION::NEGATIVE);

  bHandled |= OnAnalogMotion(positiveSemiaxis, position > 0.0f ? position : 0.0f);
  bHandled |= OnAnalogMotion(negativeSemiaxis, position < 0.0f ? -position : 0.0f);

  return bHandled;
}

void CGenericJoystickInputHandling::ProcessAxisMotions(void)
{
  for (std::map<FeatureName, FeaturePtr>::iterator it = m_features.begin(); it != m_features.end(); ++it)
    it->second->ProcessMotions();
}

bool CGenericJoystickInputHandling::OnDigitalMotion(const CDriverPrimitive& source, bool bPressed)
{
  bool bHandled = false;

  FeatureName featureName;
  if (m_buttonMap->GetFeature(source, featureName))
  {
    FeaturePtr& feature = m_features[featureName];

    if (!feature)
      feature = FeaturePtr(CreateFeature(featureName));

    if (feature)
      bHandled = feature->OnDigitalMotion(source, bPressed);
  }

  return bHandled;
}

bool CGenericJoystickInputHandling::OnAnalogMotion(const CDriverPrimitive& source, float magnitude)
{
  bool bHandled = false;

  FeatureName featureName;
  if (m_buttonMap->GetFeature(source, featureName))
  {
    FeaturePtr& feature = m_features[featureName];

    if (!feature)
      feature = FeaturePtr(CreateFeature(featureName));

    if (feature)
      bHandled = feature->OnAnalogMotion(source, magnitude);
  }

  return bHandled;
}

CJoystickFeature* CGenericJoystickInputHandling::CreateFeature(const FeatureName& featureName)
{
  CJoystickFeature* feature = nullptr;

  switch (m_buttonMap->GetFeatureType(featureName))
  {
    case FEATURE_TYPE::SCALAR:
    {
      feature = new CScalarFeature(featureName, m_handler, m_buttonMap);
      break;
    }
    case FEATURE_TYPE::ANALOG_STICK:
    {
      feature = new CAnalogStick(featureName, m_handler, m_buttonMap);
      break;
    }
    case FEATURE_TYPE::ACCELEROMETER:
    {
      feature = new CAccelerometer(featureName, m_handler, m_buttonMap);
      break;
    }
    default:
      break;
  }
  
  return feature;
}
