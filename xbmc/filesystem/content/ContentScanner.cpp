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

#include "ContentScanner.h"
#include "ContentDatabaseDirectory.h"
#include "filesystem/Directory.h"
#include "threads/SingleLock.h"
#include "utils/log.h"
#include "URL.h"

#include <queue>

using namespace XFILE;

CContentScanner::CContentScanner(void) :
  CThread("ContentScanner")
{
}

CContentScanner& CContentScanner::Get(void)
{
  static CContentScanner instance;
  return instance;
}

CContentScanner::~CContentScanner(void)
{
  m_scanEvent.Set();
}

void CContentScanner::Scan(const std::string& strPath)
{
  if (!IsRunning())
  {
    Create();
    SetPriority(GetMinPriority());
  }

  {
    CSingleLock lock(m_mutex);
    m_strPath = strPath;
  }
  m_scanEvent.Set();
}

void CContentScanner::Process(void)
{
  while (!m_bStop)
  {
    m_scanEvent.Wait();

    if (m_bStop)
      break;

    std::string strPath;
    {
      CSingleLock lock(m_mutex);
      strPath = m_strPath;
    }

    DoScan(strPath);
  }
}

void CContentScanner::DoScan(const std::string& strPath)
{
  // Breadth first scan
  std::queue<std::string> pathsToScan;
  unsigned int            pathsScanned = 0;
  unsigned int            pathsTotal = 0;

  pathsToScan.push(strPath);
  pathsTotal++;

  while (!pathsToScan.empty())
  {
    std::string strPath(pathsToScan.front());
    pathsToScan.pop();

    CFileItemList items;
    if (CDirectory::GetDirectory(strPath, items))
    {
      VECFILEITEMS vecItems = items.GetList();

      CContentDatabaseDirectory database;
      database.WriteDirectory(strPath, vecItems);

      // Add subdirectories to the queue
      for (VECFILEITEMS::const_iterator it = vecItems.begin(); it != vecItems.end(); ++it)
      {
        const CFileItemPtr& item = *it;
        const std::string& strPath = item->GetPath();

        if (item->m_bIsFolder && !strPath.empty())
        {
          pathsToScan.push(strPath);
          pathsTotal++;
        }
      }
    }

    pathsScanned++;
    CLog::Log(LOGDEBUG, "Content scaner: %u of %u paths scanned", pathsScanned, pathsTotal);
  }
}
