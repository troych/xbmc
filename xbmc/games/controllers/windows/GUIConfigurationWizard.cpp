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

#include "GUIConfigurationWizard.h"

using namespace GAME;

CGUIConfigurationWizard::CGUIConfigurationWizard(IFeatureList* featureList) :
  CThread("GUIConfigurationWizard"),
  m_features(featureList),
  m_featureIndex(0),
  m_bAborted(false)
{
}

void CGUIConfigurationWizard::Run(unsigned int featureIndex)
{
  if (IsRunning())
    Abort();

  m_featureIndex = featureIndex;

  Create();
}

bool CGUIConfigurationWizard::Abort(void)
{
  if (IsRunning())
  {
    m_bAborted = true;
    m_features->AbortPrompt();
    StopThread(true);
    return true;
  }
  return false;
}

void CGUIConfigurationWizard::Process(void)
{
  for (m_bAborted = false; !m_bAborted; m_featureIndex++)
  {
    if (!m_features->PromptForInput(m_featureIndex))
      m_bAborted = true;
  }
}
