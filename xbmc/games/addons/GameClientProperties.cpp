/*
 *      Copyright (C) 2012-2016 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GameClientProperties.h"
#include "GameClient.h"
#include "addons/IAddon.h"
#include "addons/AddonManager.h"
#include "addons/GameResource.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "settings/Settings.h"

#include <cstring>

using namespace ADDON;
using namespace GAME;
using namespace XFILE;

#define GAME_CLIENT_RESOURCES_DIRECTORY  "resources"

CGameClientProperties::CGameClientProperties(const CGameClient* parent, game_client_properties*& props)
  : m_parent(parent),
    m_properties()
{
  // Allow the caller to access the property structure directly
  props = &m_properties;
}

void CGameClientProperties::ReleaseResources(void)
{
  for (std::vector<char*>::const_iterator it = m_proxyDllPaths.begin(); it != m_proxyDllPaths.end(); ++it)
    delete[] *it;
  m_proxyDllPaths.clear();

  for (std::vector<char*>::const_iterator it = m_resourceDirectories.begin(); it != m_resourceDirectories.end(); ++it)
    delete[] *it;
  m_resourceDirectories.clear();

  for (std::vector<char*>::const_iterator it = m_extensions.begin(); it != m_extensions.end(); ++it)
    delete[] *it;
  m_extensions.clear();
}

void CGameClientProperties::InitializeProperties(void)
{
  ReleaseResources();

  m_properties.game_client_dll_path     = GetLibraryPath();
  m_properties.proxy_dll_paths          = GetProxyDllPaths();
  m_properties.proxy_dll_count          = GetProxyDllCount();
  m_properties.resource_directories     = GetResourceDirectories();
  m_properties.resource_directory_count = GetResourceDirectoryCount();
  m_properties.profile_directory        = GetProfileDirectory();
  m_properties.supports_vfs             = m_parent->SupportsVFS();
  m_properties.extensions               = GetExtensions();
  m_properties.extension_count          = GetExtensionCount();
}

const char* CGameClientProperties::GetLibraryPath(void)
{
  if (m_strLibraryPath.empty())
  {
    // Get the parent add-on's real path
    std::string strLibPath = m_parent->CAddon::LibPath();
    m_strLibraryPath = CSpecialProtocol::TranslatePath(strLibPath);
  }
  return m_strLibraryPath.c_str();
}

const char** CGameClientProperties::GetProxyDllPaths(void)
{
  if (m_proxyDllPaths.empty())
  {
    // Add all game client dependencies
    // TODO: Compare helper version with required dependency
    const ADDONDEPS& dependencies = m_parent->GetDeps();
    for (ADDONDEPS::const_iterator it = dependencies.begin(); it != dependencies.end(); ++it)
    {
      const std::string& strAddonId = it->first;
      AddonPtr addon;
      if (CAddonMgr::GetInstance().GetAddon(strAddonId, addon, ADDON_GAMEDLL))
        AddProxyDll(std::static_pointer_cast<CGameClient>(addon));
    }
  }

  if (!m_proxyDllPaths.empty())
    return const_cast<const char**>(m_proxyDllPaths.data());

  return nullptr;
}

const char** CGameClientProperties::GetResourceDirectories(void)
{
  if (m_resourceDirectories.empty())
  {
    // Add own resource directory first
    std::string path = URIUtils::AddFileToFolder(m_parent->Path(), GAME_CLIENT_RESOURCES_DIRECTORY);

    char* resourceDir = new char[path.length() + 1];
    std::strcpy(resourceDir, path.c_str());
    m_resourceDirectories.push_back(resourceDir);

    // Add all other game resource
    const ADDONDEPS& dependencies = m_parent->GetDeps();
    for (ADDONDEPS::const_iterator it = dependencies.begin(); it != dependencies.end(); ++it)
    {
      const std::string& strAddonId = it->first;
      AddonPtr addon;
      if (CAddonMgr::GetInstance().GetAddon(strAddonId, addon, ADDON_RESOURCE_GAMES))
      {
        std::shared_ptr<CGameResource> resource = std::static_pointer_cast<CGameResource>(addon);

        std::string resourcePath = resource->GetFullPath("");
        char* resourceDir = new char[resourcePath.length() + 1];
        std::strcpy(resourceDir, resourcePath.c_str());
        m_resourceDirectories.push_back(resourceDir);
      }
    }
  }

  if (!m_resourceDirectories.empty())
    return const_cast<const char**>(m_resourceDirectories.data());

  return nullptr;
}

const char* CGameClientProperties::GetProfileDirectory(void)
{
  if (m_strProfileDirectory.empty())
  {
    m_strProfileDirectory = m_parent->Profile();
    if (!CDirectory::Exists(m_strProfileDirectory))
      CDirectory::Create(m_strProfileDirectory);
  }
  return m_strProfileDirectory.c_str();
}

const char** CGameClientProperties::GetExtensions(void)
{
  for (auto& extension : m_parent->GetExtensions())
  {
    char* ext = new char[extension.length() + 1];
    std::strcpy(ext, extension.c_str());
    m_extensions.push_back(ext);
  }

  return !m_extensions.empty() ? const_cast<const char**>(m_extensions.data()) : nullptr;
}

void CGameClientProperties::AddProxyDll(const GameClientPtr& gameClient)
{
  // Get the add-on's real path
  std::string strLibPath = gameClient->CAddon::LibPath();

  // Ignore add-on if it is already added
  if (!HasProxyDll(strLibPath))
  {
    char* libPath = new char[strLibPath.length() + 1];
    std::strcpy(libPath, strLibPath.c_str());
    m_proxyDllPaths.push_back(libPath);
  }
}

bool CGameClientProperties::HasProxyDll(const std::string& strLibPath) const
{
  for (std::vector<char*>::const_iterator it = m_proxyDllPaths.begin(); it != m_proxyDllPaths.end(); ++it)
  {
    if (strLibPath == *it)
      return true;
  }
  return false;
}
