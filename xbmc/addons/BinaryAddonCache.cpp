/*
 *      Copyright (C) 2016 Team Kodi
 *      http://kodi.tv
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

#include "BinaryAddonCache.h"
#include "AddonManager.h"
#include "games/addons/GameClient.h" // TODO
#include "threads/SingleLock.h"

namespace ADDON
{

CBinaryAddonCache::~CBinaryAddonCache()
{
  Deinit();
}

void CBinaryAddonCache::Init()
{
  m_addonsToCache = {
    ADDON_AUDIODECODER,
    ADDON_INPUTSTREAM,
    ADDON_PVRDLL,
    ADDON_GAMEDLL,
  };
  CAddonMgr::GetInstance().Events().Subscribe(this, &CBinaryAddonCache::OnEvent);
  Update();
}

void CBinaryAddonCache::Deinit()
{
  CAddonMgr::GetInstance().Events().Unsubscribe(this);
}

void CBinaryAddonCache::GetAddons(VECADDONS& addons, const TYPE& type)
{
  CSingleLock lock(m_critSection);
  auto it = m_addons.find(type);

  if (it != m_addons.end())
  {
    for (auto &addon : it->second)
    {
      if (!CAddonMgr::GetInstance().IsAddonDisabled(addon->ID()))
        addons.push_back(addon);
    }
  }
}

AddonPtr CBinaryAddonCache::GetAddonInstance(const std::string& strId, TYPE type)
{
  AddonPtr addon;

  CSingleLock lock(m_critSection);

  auto it = m_addons.find(type);
  if (it != m_addons.end())
  {
    VECADDONS& addons = it->second;
    auto itAddon = std::find_if(addons.begin(), addons.end(),
      [strId](const AddonPtr& addon)
      {
        return addon->ID() == strId;
      });

    if (itAddon != addons.end())
      addon = *itAddon;
  }

  return addon;
}

void CBinaryAddonCache::OnEvent(const AddonEvent& event)
{
  if (typeid(event) == typeid(AddonEvents::InstalledChanged))
    Update();
}

void CBinaryAddonCache::Update()
{
  using AddonMap = std::multimap<TYPE, VECADDONS>;
  AddonMap addonmap;

  for (auto &addonType : m_addonsToCache)
  {
    VECADDONS addons;
    CAddonMgr::GetInstance().GetInstalledAddons(addons, addonType);
    addonmap.insert(AddonMap::value_type(addonType, addons));


    if (addonType == ADDON::ADDON_GAMEDLL)
    {
      static bool bInitialized = false;
      if (!bInitialized)
      {
        bInitialized = true;

        for (auto& addon : addons)
        {
          GAME::GameClientPtr gameClient = std::static_pointer_cast<GAME::CGameClient>(addon);

          if (gameClient->ID() != "game.libretro.reicast")
          {
            gameClient->Initialize();
            gameClient->Unload();
          }
        }
      }
    }
  }

  {
    CSingleLock lock(m_critSection);
    m_addons = std::move(addonmap);
  }
}

}