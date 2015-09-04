
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
 *
 * This Program is free software; you can redistribute it and/or modify
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

#include "ContentAddonDirectory.h"
#include "addons/AddonDatabase.h"
#include "addons/AddonManager.h"
//#include "addons/ContentAddons.h"
#include "filesystem/AddonsDirectory.h"
#include "filesystem/Directory.h"
#include "URL.h"

#include <algorithm>

using namespace ADDON;
using namespace XFILE;

bool CContentAddonDirectory::GetDirectory(const CURL& url, CFileItemList& items)
{
  bool bSuccess = false;

  if (IsRoot(url))
  {
    GetAddons(items);
    //GetSources(items); // TODO
    bSuccess = true;
  }
  else if (IsAddon(url))
  {
    CURL urlCopy(url);
    urlCopy.SetProtocol("plugin");
    if (CDirectory::GetDirectory(urlCopy.Get(), items))
    {
      bSuccess = true;

      // Reset protocol to content://
      for (int i = 0; i < items.Size(); i++)
      {
        CURL updatedPath(items[i]->GetPath());
        if (updatedPath.GetProtocol() == "plugin")
          updatedPath.SetProtocol("content");
        items[i]->SetPath(updatedPath.Get());
      }
    }
  }

  // TODO: Media sources

  /* TODO
  std::string strPath = url.Get();
  CONTENT_ADDON addon = CContentAddons::Get().GetAddonForPath(strPath);
  if (addon.get())
    bSuccess = addon->FileGetDirectory(items, strPath);
  */
  return bSuccess;
}

bool CContentAddonDirectory::Exists(const char* strPath)
{
  bool bExists = false;
  AddonPtr dummy;

  CURL url(strPath);
  if (IsRoot(url))
    bExists = true;
  else if (IsAddon(url))
    bExists = true;

  // TODO: Media sources

  return bExists;
}

bool CContentAddonDirectory::GetAddons(VECADDONS& addons)
{
  GetAddons("game", addons);
  GetAddons("photo", addons);
  return !addons.empty();
}

bool CContentAddonDirectory::GetAddons(const std::string& content, VECADDONS& addons)
{
  TYPE type = TranslateContentType(content);
  if (type != ADDON_UNKNOWN)
    CAddonMgr::Get().GetAddons(type, addons);
  return !addons.empty();
}

bool CContentAddonDirectory::HasInstallableAddons(void)
{
  bool bHasInstallable = false;

  VECADDONS installableAddons;
  CAddonDatabase database;
  if (database.Open() && database.GetAddons(installableAddons))
  {
    if (std::find_if(installableAddons.begin(), installableAddons.end(),
        [](const AddonPtr& pAddon)
        {
          return pAddon->IsType(ADDON_GAME_CONTENT) &&
                 !CAddonMgr::Get().IsAddonDisabled(pAddon->ID()) &&
                 !CAddonMgr::Get().IsAddonInstalled(pAddon->ID()) &&
                 CAddonMgr::Get().CanAddonBeInstalled(pAddon);
        }) != installableAddons.end())
    {
      bHasInstallable = true;
    }
  }

  return bHasInstallable;
}

bool CContentAddonDirectory::IsRoot(const CURL& url)
{
  return url.GetHostName().empty();
}

bool CContentAddonDirectory::IsAddon(const CURL& url)
{
  AddonPtr dummy;
  return CAddonMgr::Get().GetAddon(url.GetHostName(), dummy) && dummy;
}

bool CContentAddonDirectory::GetAddons(CFileItemList& items)
{
  VECADDONS addons;
  if (GetAddons(addons))
  {
    for (IVECADDONS it = addons.begin(); it != addons.end(); ++it)
    {
      const AddonPtr& addon = *it;
      items.Add(CAddonsDirectory::FileItemFromAddon(addon, "content://" + addon->ID() + "/", true));
    }
  }
  return !items.IsEmpty();
}
