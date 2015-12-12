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

namespace GAME
{
  class CGUIControllerWindow;

  class CGUIConfigurationUtility : public IConfigurationUtility
  {
  public:
    CGUIConfigurationUtility(CGUIControllerWindow* window);

    virtual ~CGUIConfigurationUtility(void);

    // implementation of IConfigurationUtility
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual void OnControllerFocused(unsigned int controllerIndex) override;
    virtual void OnControllerSelected(unsigned int controllerIndex) override;
    virtual void OnFeatureFocused(unsigned int featureIndex) override;
    virtual void OnFeatureSelected(unsigned int featureIndex) override;

  private:
    IFeatureList*    m_featureList;
    IControllerList* m_controllerList;
  };
}
