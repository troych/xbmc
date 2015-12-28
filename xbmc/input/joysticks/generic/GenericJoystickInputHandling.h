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

#include <string>
#include <vector>

namespace JOYSTICK
{
  class IJoystickInputHandler;
  class IJoystickButtonMap;

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
    bool ProcessHatDirection(int index, HAT_STATE oldState, HAT_STATE newState, HAT_DIRECTION targetDir);

    bool OnPress(const FeatureName& feature);
    void OnRelease(const FeatureName& feature);

    void StartDigitalRepeating(const FeatureName& feature);
    void StopDigitalRepeating(const FeatureName& feature);

    float GetAxisState(int axisIndex) const;

    struct ButtonEvent
    {
      bool state;
      bool bHandled; // Needed to absorb keyboard repeat events
    };

    IJoystickInputHandler* const m_handler;
    IJoystickButtonMap* const    m_buttonMap;
    std::vector<ButtonEvent>     m_buttonStates;
    std::vector<HAT_STATE>       m_hatStates;
    std::vector<float>           m_axisStates;
    std::vector<FeatureName> m_featuresWithMotion;
    std::vector<FeatureName> m_repeatingFeatures;
  };
}
