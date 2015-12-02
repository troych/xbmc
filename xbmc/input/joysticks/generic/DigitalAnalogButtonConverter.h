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

#include "input/joysticks/IJoystickInputHandler.h"

#include <vector>

namespace JOYSTICK
{
  /*!
   * \brief Convert between digital and analog button events
   *
   * If the input type is digital, driver axes are converted to digital buttons by
   * thresholding around 0.5.
   *
   * If the input type is analog, driver buttons/hats are converted to analog
   * events with magnitude 0.0 or 1.0.
   */
  class CDigitalAnalogButtonConverter : public IJoystickInputHandler
  {
  public:
    CDigitalAnalogButtonConverter(IJoystickInputHandler* handler);

    // implementation of IJoystickInputHandler
    virtual std::string ControllerID(void) const override;
    virtual InputType GetInputType(const FeatureName& feature) const override;
    virtual bool OnButtonPress(const FeatureName& feature, bool bPressed) override;
    virtual bool OnButtonMotion(const FeatureName& feature, float magnitude) override;
    virtual bool OnAnalogStickMotion(const FeatureName& feature, float x, float y) override;
    virtual bool OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z) override;

  private:
    bool IsActivated(const FeatureName& feature) const;
    void Activate(const FeatureName& feature);
    void Deactivate(const FeatureName& feature);

    IJoystickInputHandler* const  m_handler;
    std::vector<FeatureName>  m_activatedFeatures; // for tracking analog features mapped to digital input
  };
}
