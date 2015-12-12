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
#include "input/joysticks/JoystickTypes.h"

#include <vector>

#define DEFAULT_CONTROLLER_ID    "game.controller.default"

namespace JOYSTICK
{
  class IButtonKeyHandler;
  class IButtonSequence;
}

namespace GAME
{
  /*!
   * \brief Implementation of IJoystickInputHandler for the default controller
   *
   * \sa IJoystickInputHandler
   */
  class CDefaultController : public JOYSTICK::IJoystickInputHandler
  {
  public:
    CDefaultController(void);

    virtual ~CDefaultController(void);

    // implementation of IJoystickInputHandler
    virtual std::string ControllerID(void) const override;
    virtual JOYSTICK::InputType GetInputType(const JOYSTICK::FeatureName& feature) const override;
    virtual bool OnButtonPress(const JOYSTICK::FeatureName& feature, bool bPressed) override;
    virtual bool OnButtonMotion(const JOYSTICK::FeatureName& feature, float magnitude) override;
    virtual bool OnAnalogStickMotion(const JOYSTICK::FeatureName& feature, float x, float y) override;
    virtual bool OnAccelerometerMotion(const JOYSTICK::FeatureName& feature, float x, float y, float z) override;

  private:
    /*!
     * \brief Get the button key, as defined in guilib/Key.h, for the specified
     *        joystick feature/direction
     *
     * \return The button key ID
     */
    static unsigned int GetButtonKeyID(const JOYSTICK::FeatureName& feature, JOYSTICK::CARDINAL_DIRECTION dir = JOYSTICK::CARDINAL_DIRECTION::UNKNOWN);

    /*!
     * \brief Return a vector of the four cardinal directions
     */
    static const std::vector<JOYSTICK::CARDINAL_DIRECTION>& GetDirections(void);

    JOYSTICK::IButtonKeyHandler* const m_handler;
    JOYSTICK::IButtonSequence*         m_easterEgg;
  };
}
