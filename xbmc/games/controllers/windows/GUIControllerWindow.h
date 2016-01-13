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
#include "guilib/GUIDialog.h"

namespace GAME
{
  class CGUIControllerWindow : public CGUIDialog,
                               public IConfigurationWindow
  {
  public:
    CGUIControllerWindow(void);
    virtual ~CGUIControllerWindow(void);

    // implementation of CGUIControl via CGUIDialog
    virtual bool OnMessage(CGUIMessage& message) override;

    // implementation of IConfigurationWindow
    virtual void FocusController(unsigned int controllerIndex) override;
    virtual void FocusFeature(unsigned int featureIndex) override;
    virtual void SetLabel(unsigned int featureIndex, const std::string& strLabel) override;

  protected:
    // implementation of CGUIWindow via CGUIDialog
    virtual void OnInitWindow(void) override;
    virtual void OnDeinitWindow(int nextWindowID) override;

  private:
    void OnControllerFocused(unsigned int controllerIndex);
    void OnControllerSelected(unsigned int controllerIndex);
    void OnFeatureFocused(unsigned int featureIndex);
    void OnFeatureSelected(unsigned int featureIndex);

    void GetMoreControllers(void);
    void ResetController(void);

    IControllerList* m_controllerList;
    IFeatureList*    m_featureList;
  };
}
