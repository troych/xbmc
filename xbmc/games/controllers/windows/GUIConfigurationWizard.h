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
#include "input/joysticks/IJoystickButtonMapper.h"
#include "threads/Thread.h"
#include "utils/Observer.h"

namespace GAME
{
  class CGUIConfigurationWizard : public IConfigurationWizard,
                                  public JOYSTICK::IJoystickButtonMapper,
                                  public CThread,
                                  public Observer
  {
  public:
    CGUIConfigurationWizard(IFeatureList* featureList);

    virtual ~CGUIConfigurationWizard(void) { }

    // implementation of IConfigurationWizard
    virtual void Run(unsigned int featureIndex) override;
    virtual bool IsWizardRunning(void) const override;
    virtual bool Abort(void) override;

    // implementation of IJoystickButtonMapper
    virtual std::string ControllerID(void) const override;
    virtual bool MapPrimitive(JOYSTICK::IJoystickButtonMap* buttonMap, const JOYSTICK::CDriverPrimitive& primitive) override;

    // implementation of Observer
    virtual void Notify(const Observable& obs, const ObservableMessage msg) override;

  protected:
    // implementation of CThread
    virtual void Process(void) override;

  private:
    void InstallHooks(void);
    void RemoveHooks(void);

    IFeatureList* const m_features;
    JOYSTICK::IJoystickButtonMapper* m_buttonMapper;
    unsigned int        m_featureIndex;
    bool                m_bAborted;
  };
}
