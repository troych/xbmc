/*
 *      Copyright (C) 2015 Team XBMC
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

#include "ControllerManager.h"
#include "Controller.h"
#include "DefaultController.h"
#include "addons/AddonManager.h"

using namespace ADDON;
using namespace GAME;

CControllerManager& CControllerManager::GetInstance(void)
{
  static CControllerManager instance;
  return instance;
}

void CControllerManager::Start(void)
{
  UpdateAddons();
  CAddonMgr::Get().RegisterObserver(this);
}

void CControllerManager::Stop(void)
{
  CAddonMgr::Get().UnregisterObserver(this);
  ClearAddons();
}

bool CControllerManager::GetController(const std::string& strControllerId, ControllerPtr& addon) const
{
  CSingleLock lock(m_critSection);

  for (const ControllerPtr& controller : m_controllers)
  {
    if (controller->ID() == strControllerId)
    {
      addon = controller;
      return true;
    }
  }

  return false;
}

ControllerVector CControllerManager::GetControllers(void) const
{
  CSingleLock lock(m_critSection);
  return m_controllers;
}

void CControllerManager::UpdateAddons(void)
{
  VECADDONS addons;
  if (CAddonMgr::Get().GetAddons(ADDON_GAME_CONTROLLER, addons, true))
  {
    CSingleLock lock(m_critSection);

    // Look for new controllers
    ControllerVector newControllers;
    for (AddonPtr& addon : addons)
    {
      ControllerPtr controller = std::dynamic_pointer_cast<CController>(addon);
      if (controller && std::find(m_controllers.begin(), m_controllers.end(), controller) == m_controllers.end())
        newControllers.push_back(controller);
    }

    // Remove old controllers
    for (ControllerVector::iterator it = m_controllers.begin(); it != m_controllers.end(); /* ++it */)
    {
      if (std::find(newControllers.begin(), newControllers.end(), *it) == newControllers.end())
        it = m_controllers.erase(it); // Not found, remove it
      else
        ++it;
    }

    // Add new controllers
    for (ControllerPtr& newController : newControllers)
    {
      if (newController->LoadLayout())
        m_controllers.push_back(newController);
    }

    // Sort add-ons
    std::sort(m_controllers.begin(), m_controllers.end(),
      [](const ControllerPtr& i, const ControllerPtr& j)
      {
        if (i->ID() == DEFAULT_CONTROLLER_ID)
          return true;
        if (j->ID() == DEFAULT_CONTROLLER_ID)
          return false;
        return i->ID() < j->ID();
      });
  }
}

void CControllerManager::ClearAddons(void)
{
  CSingleLock lock(m_critSection);
  m_controllers.clear();
}

void CControllerManager::Notify(const Observable& obs, const ObservableMessage msg)
{
  if (msg == ObservableMessageAddons)
    UpdateAddons();
}
