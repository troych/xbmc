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

#include "IConfigurationWindow.h"
#include "games/controllers/ControllerFeature.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMapper.h"
#include "input/joysticks/JoystickTypes.h"
#include "threads/Event.h"
#include "utils/Observer.h"

namespace GAME
{
  class CGUIFeatureButton : public IFeatureButton,
                            public JOYSTICK::IJoystickButtonMapper,
                            public Observer
  {
  public:
    CGUIFeatureButton(IConfigurationWindow* window,
                      const CControllerFeature& feature,
                      const std::string& strControllerId,
                      unsigned int featureIndex);
    virtual ~CGUIFeatureButton(void) { }

    // implementation of IFeatureButton
    virtual bool PromptForInput(void) override;
    virtual void Abort(void) override;

    // implementation of IJoystickButtonMapper
    virtual std::string ControllerID(void) const override { return m_strControllerId; }
    virtual bool IsMapping(void) const override { return m_bIsMapping; }
    virtual bool MapPrimitive(JOYSTICK::IJoystickButtonMap* buttonMap, const JOYSTICK::CDriverPrimitive& primitive) override;

    // implementation of Observer
    virtual void Notify(const Observable& obs, const ObservableMessage msg) override;

  private:
    void PromptButton(void);
    void PromptAnalogStick(void);

    void SetLabel(const std::string& strPromptMsg);

    void InstallHooks(void);
    void RemoveHooks(void);

    IConfigurationWindow* const  m_window;
    const std::string            m_strControllerId;
    const CControllerFeature     m_feature;
    const unsigned int           m_featureIndex;
    bool                         m_bIsMapping;
    JOYSTICK::CARDINAL_DIRECTION m_analogStickDirection;
    JOYSTICK::CDriverPrimitive   m_lastPrimtive; // Equal to the most recent primitive being mapped
    bool                         m_bAborted;
    CEvent                       m_waitCondition;
  };
}
