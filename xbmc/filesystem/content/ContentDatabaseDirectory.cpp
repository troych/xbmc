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

#include "ContentDatabaseDirectory.h"
#include "ContentDatabaseFilter.h"
#include "dbwrappers/nosql/IDocument.h"
#include "dbwrappers/nosql/NoSqlIterator.h"
#include "filesystem/content/schema/Directory.h"
#include "games/tags/GameInfoTag.h"
#include "utils/URIUtils.h"
#include "GUIInfoManager.h"
#include "URL.h"

#include <algorithm>

using namespace dbiplus;
using namespace XFILE;
using namespace XFILE::DB;

CNoSqlDatabase CContentDatabaseDirectory::m_db;

bool CContentDatabaseDirectory::GetDirectory(const CURL& url, CFileItemList& items)
{
  const std::string& strCollection = url.GetHostName();

  if (strCollection.empty())
  {
    // TODO: Ask m_db for the collections provided by all of its factories
    return true;
  }
  else
  {
    const std::string& strPath = url.GetFileName();
    if (!strPath.empty())
    {
      // TODO: Handle a filename before the query
      return false;
    }
    else
    {
      CContentDatabaseFilter filter(strCollection, url.GetOption("q"));
      return GetDirectory(filter, items);
    }
  }

  return false;
}

bool CContentDatabaseDirectory::Exists(const CURL& url)
{
  const std::string& strCollection = url.GetHostName();

  if (strCollection.empty())
    return true;
  
  CContentDatabaseFilter filter(MediaTypes::FromString(strCollection), url.GetOption("q"));
  if (filter.Type() != MediaTypeNone)
  {
    NoSqlIteratorPtr it = m_db.GetDocuments(filter.Type(), filter.Value());
    return it && it->IsValid();
  }

  return false;
}

bool CContentDatabaseDirectory::GetDirectory(const CContentDatabaseFilter& filter, CFileItemList& items)
{
  NoSqlIteratorPtr it = m_db.GetDocuments(filter.Type(), filter.Value());
  if (it)
  {
    while (it->IsValid())
    {
      CFileItemPtr item = std::static_pointer_cast<CFileItem>(it->GetRecord(true));
      if (item)
        items.Add(item);
    }
    return true;
  }
  return false;
}

bool CContentDatabaseDirectory::WriteDirectory(const std::string& url, const VECFILEITEMS& items)
{
  VECFILEITEMS oldFiles;
  VECFILEITEMS newFiles;
  VECFILEITEMS orphanedFiles;
  std::set<std::string> oldDirectories;
  std::set<std::string> newDirectories;
  std::vector<std::string> orphanedDirectories;

  GetDirectoryContents(items, newFiles, newDirectories);
  GetDatabaseContents(url, oldFiles, oldDirectories);

  // Merge and get orphaned files
  MergeFiles(oldFiles, newFiles, orphanedFiles);

  // Get orphaned directories
  std::set_difference(oldDirectories.begin(), oldDirectories.end(),
                      newDirectories.begin(), newDirectories.end(),
                      std::back_inserter(orphanedDirectories));

  // Process new files
  for (VECFILEITEMS::const_iterator it = newFiles.begin(); it != newFiles.end(); ++it)
  {
    m_db.Put("file", it->get());
    if ((*it)->HasGameInfoTag())
      m_db.Put("game", (*it)->GetGameInfoTag());
  }

  // Process orphaned files
  for (VECFILEITEMS::const_iterator it = orphanedFiles.begin(); it != orphanedFiles.end(); ++it)
  {
    //(*it)->SetOrphaned(true); // TODO
    m_db.Put("file", it->get());
  }

  // Process new directories

  // Process old directories
  for (std::vector<std::string>::const_iterator it = orphanedDirectories.begin(); it != orphanedDirectories.end(); ++it)
  {
    std::string strPath = URIUtils::AddFileToFolder(url, *it);
    //WriteDirectoryStatus(strPath, false); // TODO
    // - OR -
    //DisableDirectory(strPath); // TODO
  }

  // Record directory
  //return WriteDirectoryContents(url, newFiles, newDirectories); // TODO

  // Number of items likely changed
  g_infoManager.SetLibraryBool(LIBRARY_HAS_GAMES, Exists(CURL("contentdb://game/")));

  return true;
}

void CContentDatabaseDirectory::GetDirectoryContents(const VECFILEITEMS& items, VECFILEITEMS& files, std::set<std::string>& directories)
{
  for (VECFILEITEMS::const_iterator itItem = items.begin(); itItem != items.end(); ++itItem)
  {
    if (!(*itItem)->m_bIsFolder)
    {
      files.push_back(*itItem);
    }
    else
    {
      std::string directoryUrl = (*itItem)->GetPath();
      URIUtils::RemoveSlashAtEnd(directoryUrl);
      directories.insert(URIUtils::GetFileName(directoryUrl));
    }
  }
}

void CContentDatabaseDirectory::GetDatabaseContents(const std::string& url, VECFILEITEMS& files, std::set<std::string>& directories)
{
  DirectoryPtr directory = std::static_pointer_cast<CDirectory>(m_db.GetDocumentByProperty("directory", "url", url));
  if (directory)
  {
    // Process files
    const std::vector<int64_t>& fileIds = directory->FileIDs();
    for (std::vector<int64_t>::const_iterator it = fileIds.begin(); it != fileIds.end(); ++it)
    {
      CFileItemPtr file = std::static_pointer_cast<CFileItem>(m_db.GetDocumentByID("file", *it));
      if (file)
        files.push_back(file);
    }

    // Process directories
    directories.swap(directory->DirectoryNames());
  }
}

void CContentDatabaseDirectory::MergeFiles(VECFILEITEMS& oldFiles, VECFILEITEMS& newFiles, VECFILEITEMS& orphanedFiles)
{
  for (IVECFILEITEMS itNew = newFiles.begin(); itNew != newFiles.end(); ++itNew)
  {
    CFileItemPtr& newFile = *itNew;

    std::string strPath = newFile->GetPath();
    URIUtils::RemoveSlashAtEnd(strPath);

    auto findByPath = [&strPath](const CFileItemPtr& file)
    {
      return file->GetPath() == strPath;
    };

    VECFILEITEMS::iterator itOld = std::find_if(oldFiles.begin(), oldFiles.end(), findByPath);

    if (itOld != oldFiles.end())
    {
      CFileItemPtr& oldFile = *itOld;
      newFile.swap(oldFile);
      newFile->Merge(*oldFile);
      oldFiles.erase(itOld);
    }
    else
    {
      newFile->SetPath(strPath);
    }
  }
  orphanedFiles.swap(oldFiles);
}

void CContentDatabaseDirectory::RegisterFactory(dbiplus::IDocumentFactory* factory)
{
  m_db.RegisterFactory(factory);
}

void CContentDatabaseDirectory::UnregisterFactory(dbiplus::IDocumentFactory* factory)
{
  m_db.UnregisterFactory(factory);
}
