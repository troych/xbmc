/*
 *      Copyright (C) 2012-2014 Team XBMC
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

#include "GUIWindowGames.h"
#include "addons/GUIDialogAddonInfo.h"
#include "addons/GUIWindowAddonBrowser.h"
#include "Application.h"
#include "dialogs/GUIDialogProgress.h"
#include "FileItem.h"
#include "filesystem/content/ContentScanner.h"
#include "games/tags/GameInfoTag.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/WindowIDs.h"
#include "GUIPassword.h"
#include "input/Key.h"
#include "settings/Settings.h"
#include "URL.h"
#include "Util.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"

#define CONTROL_BTNVIEWASICONS      2
#define CONTROL_BTNSORTBY           3
#define CONTROL_BTNSORTASC          4
//#define CONTROL_LABELFILES         12

using namespace XFILE;

CGUIWindowGames::CGUIWindowGames() : CGUIMediaWindow(WINDOW_GAMES, "MyGames.xml")
{
}

bool CGUIWindowGames::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_INIT:
    {
      m_rootDir.AllowNonLocalSources(false);

      // Is this the first time the window is opened?
      if (m_vecItems->GetPath() == "?" && message.GetStringParam().empty())
        m_vecItems->SetPath("");

      m_dlgProgress = dynamic_cast<CGUIDialogProgress*>(g_windowManager.GetWindow(WINDOW_DIALOG_PROGRESS));
    }
    break;

  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (m_viewControl.HasControl(iControl))  // list/thumb control
      {
        int iItem = m_viewControl.GetSelectedItem();
        CFileItemPtr pItem = m_vecItems->Get(iItem);
        int iAction = message.GetParam1();

        if (iAction == ACTION_DELETE_ITEM)
        {
          if (CSettings::Get().GetBool("filelists.allowfiledeletion"))
          {
            OnDeleteItem(iItem);
            return true;
          }
        }
        else if (iAction == ACTION_PLAYER_PLAY)
        {
          return OnClick(iItem);
        }
        else if (iAction == ACTION_SHOW_INFO)
        {
          if (!m_vecItems->IsPlugin() && pItem && (pItem->IsPlugin() || pItem->IsScript()))
          {
            CGUIDialogAddonInfo::ShowForItem(pItem);
            return true;
          }
        }
      }
    }
    break;
  }
  return CGUIMediaWindow::OnMessage(message);
}

void CGUIWindowGames::SetupShares()
{
  CGUIMediaWindow::SetupShares();
  // XBMC downloads a list of supported extensions from the remote add-ons
  // repo. Zip files are treated as directories and scanned recursively; if
  // they don't contain valid extensions (such as MAME arcade games), the
  // entire zip will be missing from the MyGames window. Skipping the recursive
  // scan always shows zip files (note: entering the zip will show an empty
  // folder) and speeds up directory listing as a nice side effect.
  m_rootDir.SetFlags(DIR_FLAG_NO_FILE_DIRS);
}

bool CGUIWindowGames::GetDirectory(const std::string& strDirectory, CFileItemList& items)
{
  if (!CGUIMediaWindow::GetDirectory(strDirectory, items))
    return false;

  /*
  std::string label;
  if (items.GetLabel().empty() && m_rootDir.IsSource(items.GetPath(), CMediaSourceSettings::Get().GetSources("pictures"), &label)) 
    items.SetLabel(label);
  */

  return true;
}

void CGUIWindowGames::GetContextButtons(int itemNumber, CContextButtons &buttons)
{
  CFileItemPtr item = m_vecItems->Get(itemNumber);

  if (item && !item->GetProperty("pluginreplacecontextitems").asBoolean())
  {
    if (m_vecItems->IsSourcesPath())
    {
      // Context buttons for a sources path, like "Add source", "Remove Source", etc.
      CGUIDialogContextMenu::GetContextButtons("games", item, buttons);
    }
    else if (!m_vecItems->IsVirtualDirectoryRoot())
    {
      if (item->IsGame())
      {
        buttons.Add(CONTEXT_BUTTON_PLAY_ITEM, 208); // Play
        // Let RetroPlayer handle this one
        //buttons.Add(CONTEXT_BUTTON_PLAY_WITH, 15213); // Play With...
      }

      if (URIUtils::IsContent(item->GetPath()))
        buttons.Add(CONTEXT_BUTTON_ADD_TO_LIBRARY, 27026);

      if (!m_vecItems->IsPlugin() &&
          (item->IsPlugin() ||
           item->IsScript() ||
           URIUtils::IsContent(item->GetPath())))
      {
        buttons.Add(CONTEXT_BUTTON_INFO, 24003); // Add-on info
      }

      if (CSettings::Get().GetBool("filelists.allowfiledeletion") && !item->IsReadOnly())
      {
        buttons.Add(CONTEXT_BUTTON_DELETE, 117);
        buttons.Add(CONTEXT_BUTTON_RENAME, 118);
      }

      if (item->IsPlugin() || item->IsScript() || m_vecItems->IsPlugin() ||
          URIUtils::IsContent(item->GetPath()))
      {
        buttons.Add(CONTEXT_BUTTON_PLUGIN_SETTINGS, 1045);
      }
    }
    else
    {
      // We are virtual directory root
      buttons.Add(CONTEXT_BUTTON_SWITCH_MEDIA, 523); // Switch media
    }
  }

  CGUIMediaWindow::GetContextButtons(itemNumber, buttons);

  if (item && !item->GetProperty("pluginreplacecontextitems").asBoolean())
    buttons.Add(CONTEXT_BUTTON_SETTINGS, 5); // Settings
}

bool CGUIWindowGames::OnContextButton(int itemNumber, CONTEXT_BUTTON button)
{
  CFileItemPtr item = m_vecItems->Get(itemNumber);
  if (item)
  {
    if (m_vecItems->IsSourcesPath())
    {
      if (CGUIDialogContextMenu::OnContextButton("games", item, button))
      {
        Update("sources://games/");
        return true;
      }
    }

    switch (button)
    {
    case CONTEXT_BUTTON_PLAY_ITEM:
    case CONTEXT_BUTTON_PLAY_WITH:
      PlayGame(*item);
      return true;
    case CONTEXT_BUTTON_INFO:
      OnInfo(itemNumber);
      return true;
    case CONTEXT_BUTTON_DELETE:
      OnDeleteItem(itemNumber);
      return true;
    case CONTEXT_BUTTON_RENAME:
      OnRenameItem(itemNumber);
      return true;
    case CONTEXT_BUTTON_SETTINGS:
      g_windowManager.ActivateWindow(WINDOW_SETTINGS_MYGAMES);
      return true;
    case CONTEXT_BUTTON_ADD_TO_LIBRARY:
      ScanToLibrary(*item);
      return true;
    default:
      break;
    }
  }
  return CGUIMediaWindow::OnContextButton(itemNumber, button);
}

bool CGUIWindowGames::OnClick(int itemNumber)
{
  CFileItemPtr item = m_vecItems->Get(itemNumber);
  if (!item)
    return true;

  /* TODO
  if (item->GetPath() == "library://game/more.xml/")
  {
    std::string strAddonID;
    if (1 == CGUIWindowAddonBrowser::SelectAddonID(ADDON::ADDON_GAME_CONTENT, strAddonID, false, true, false, true, false))
    {
      const std::string strDirectory = "content://" + strAddonID + "/";
      Update(strDirectory);
    }
    return true;
  }
  */

  if (!(item->m_bIsFolder || item->IsFileFolder()) && item->IsGame())
    return PlayGame(*item);
  else
    return CGUIMediaWindow::OnClick(itemNumber);
}

void CGUIWindowGames::OnInfo(int itemNumber)
{
  CFileItemPtr item = m_vecItems->Get(itemNumber);
  if (!item)
    return;

  if (!m_vecItems->IsPlugin() && (item->IsPlugin() || item->IsScript()))
    CGUIDialogAddonInfo::ShowForItem(item);
}

bool CGUIWindowGames::PlayGame(const CFileItem &item)
{
  // Decode zip:// path for zip root directory
  CFileItem gameFile = item;
  CURL url(item.GetPath());
  if (url.GetProtocol() == "zip" && url.GetFileName() == "")
    gameFile.SetPath(url.GetHostName());

  // Allocate a game info tag to let the player know it's a game
  gameFile.GetGameInfoTag();

  // Let RetroPlayer choose the right action
  return g_application.PlayFile(gameFile) == PLAYBACK_OK;
}

std::string CGUIWindowGames::GetStartFolder(const std::string &dir)
{
  if (StringUtils::EqualsNoCase(dir, "plugins") ||
      StringUtils::EqualsNoCase(dir, "addons"))
  {
    return "addons://sources/game/";
  }

  // Make sure our shares are configured
  SetupShares();

  VECSOURCES shares;
  m_rootDir.GetSources(shares);

  bool bIsSourceName = false;
  int iIndex = CUtil::GetMatchingSource(dir, shares, bIsSourceName);
  if (iIndex >= 0)
  {
    if (iIndex < (int)shares.size() && shares[iIndex].m_iHasLock == 2)
    {
      CFileItem item(shares[iIndex]);
      if (!g_passwordManager.IsItemUnlocked(&item, "games"))
        return "";
    }

    if (bIsSourceName)
      return shares[iIndex].strPath;

    return dir;
  }

  return CGUIMediaWindow::GetStartFolder(dir);
}

void CGUIWindowGames::ScanToLibrary(const CFileItem& item)
{
  CContentScanner::Get().Scan(item.GetPath());
}
