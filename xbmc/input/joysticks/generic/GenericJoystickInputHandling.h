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
#include "input/joysticks/JoystickTypes.h"

#include <map>
#include <memory>

namespace JOYSTICK
{
  class CDriverPrimitive;
  class IJoystickInputHandler;
  class IJoystickButtonMap;

  class CJoystickFeature
  {
  public:
    CJoystickFeature(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap);
    virtual ~CJoystickFeature(void) { }

    virtual bool OnDigitalMotion(const CDriverPrimitive& source, bool bPressed) = 0;
    virtual bool OnAnalogMotion(const CDriverPrimitive& source, float magnitude) = 0;
    virtual void ProcessMotions(void) = 0;

  protected:
    const FeatureName            m_name;
    IJoystickInputHandler* const m_handler;
    IJoystickButtonMap* const    m_buttonMap;
  };

  typedef std::shared_ptr<CJoystickFeature> FeaturePtr;

  class CScalarFeature : public CJoystickFeature
  {
  public:
    CScalarFeature(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap);
    virtual ~CScalarFeature(void) { }

    // implementation of CJoystickFeature
    virtual bool OnDigitalMotion(const CDriverPrimitive& source, bool bPressed) override;
    virtual bool OnAnalogMotion(const CDriverPrimitive& source, float magnitude) override;
    virtual void ProcessMotions(void) override { }

  private:
    const INPUT_TYPE m_inputType;
    bool             m_bDigitalState;
    float            m_analogState;
  };

  class CFeatureAxis
  {
  public:
    CFeatureAxis(void);

    void SetPositiveDistance(float distance) { m_positiveDistance = distance; }
    void SetNegativeDistance(float distance) { m_negativeDistance = distance; }

    float GetPosition(void) const;
    void Reset(void);

  protected:
    float m_positiveDistance;
    float m_negativeDistance;
  };

  class CAnalogStick : public CJoystickFeature
  {
  public:
    CAnalogStick(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap);
    virtual ~CAnalogStick(void) { }

    // implementation of CJoystickFeature
    virtual bool OnDigitalMotion(const CDriverPrimitive& source, bool bPressed) override;
    virtual bool OnAnalogMotion(const CDriverPrimitive& source, float magnitude) override;
    virtual void ProcessMotions(void) override;

  protected:
    CFeatureAxis m_vertAxis;
    CFeatureAxis m_horizAxis;

    float m_vertState;
    float m_horizState;
  };

  class CAccelerometer : public CJoystickFeature
  {
  public:
    CAccelerometer(const FeatureName& name, IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap);
    virtual ~CAccelerometer(void) { }

    // implementation of CJoystickFeature
    virtual bool OnDigitalMotion(const CDriverPrimitive& source, bool bPressed) override;
    virtual bool OnAnalogMotion(const CDriverPrimitive& source, float magnitude) override;
    virtual void ProcessMotions(void) override;

  protected:
    CFeatureAxis m_xAxis;
    CFeatureAxis m_yAxis;
    CFeatureAxis m_zAxis;

    float m_xAxisState;
    float m_yAxisState;
    float m_zAxisState;
  };

  /*!
   * \brief Class to translate input from the driver into higher-level features
   *
   * Raw driver input arrives for three elements: buttons, hats and axes. When
   * driver input is handled by this class, it translates the raw driver elements
   * into physical joystick features, such as buttons, analog sticks, etc.
   *
   * The provided button map instructs this class on how driver input should be
   * mapped to higher-level features. The button map has been abstracted away
   * behind the IJoystickButtonMap interface so that it can be provided by an
   * add-on.
   */
  class CGenericJoystickInputHandling : public IJoystickDriverHandler
  {
  public:
    CGenericJoystickInputHandling(IJoystickInputHandler* handler, IJoystickButtonMap* buttonMap);

    virtual ~CGenericJoystickInputHandling(void);

    // implementation of IJoystickDriverHandler
    virtual bool OnButtonMotion(unsigned int buttonIndex, bool bPressed) override;
    virtual bool OnHatMotion(unsigned int hatIndex, HAT_STATE state) override;
    virtual bool OnAxisMotion(unsigned int axisIndex, float position) override;
    virtual void ProcessAxisMotions(void) override;

  private:
    bool OnDigitalMotion(const CDriverPrimitive& source, bool bPressed);
    bool OnAnalogMotion(const CDriverPrimitive& source, float magnitude);

    CJoystickFeature* CreateFeature(const FeatureName& featureName);

    IJoystickInputHandler* const m_handler;
    IJoystickButtonMap* const    m_buttonMap;

    std::map<FeatureName, FeaturePtr> m_features;
  };
}
