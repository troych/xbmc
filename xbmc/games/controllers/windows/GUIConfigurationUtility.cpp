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

#include "GUIConfigurationUtility.h"
#include "GUIControllerList.h"
#include "GUIControllerWindow.h"
#include "GUIFeatureList.h"

using namespace GAME;

CGUIConfigurationUtility::CGUIConfigurationUtility(CGUIControllerWindow* window) :
  m_featureList(new CGUIFeatureList(window)),
  m_controllerList(new CGUIControllerList(window, m_featureList))
{
}

CGUIConfigurationUtility::~CGUIConfigurationUtility(void)
{
  delete m_controllerList;
  delete m_featureList;
}

bool CGUIConfigurationUtility::Initialize(void)
{
  return m_featureList->Initialize() &&
         m_controllerList->Initialize();
}

void CGUIConfigurationUtility::Deinitialize(void)
{
  m_featureList->Deinitialize();
  m_controllerList->Deinitialize();
}

void CGUIConfigurationUtility::OnControllerFocused(unsigned int controllerIndex)
{
  m_controllerList->OnFocus(controllerIndex);
}

void CGUIConfigurationUtility::OnControllerSelected(unsigned int controllerIndex)
{
  m_controllerList->OnSelect(controllerIndex);
}

void CGUIConfigurationUtility::OnFeatureFocused(unsigned int featureIndex)
{
  m_featureList->OnFocus(featureIndex);
}

void CGUIConfigurationUtility::OnFeatureSelected(unsigned int featureIndex)
{
  m_featureList->OnSelect(featureIndex);
}
