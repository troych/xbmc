#pragma once
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
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

#include "dbwrappers/nosql/NoSqlDatabase.h"
#include "filesystem/IDirectory.h"
#include "media/MediaType.h"
#include "FileItem.h"

#include <string>

class CURL;

namespace XFILE
{
  class CContentDatabaseFilter;

  class CContentDatabaseDirectory : public IDirectory//, public IDirectoryFactory // TODO
  {
  public:
    CContentDatabaseDirectory(void) { }
    virtual ~CContentDatabaseDirectory(void) { }

    // implementation of IDirectory
    virtual bool GetDirectory(const CURL& url, CFileItemList& items) override;
    virtual bool IsAllowed(const CURL& url) const override { return true; }
    virtual bool Exists(const CURL& url) override;

    bool GetDirectory(const CContentDatabaseFilter& filter, CFileItemList& items);

    bool WriteDirectory(const std::string& url, const VECFILEITEMS& items);

    static void RegisterFactory(dbiplus::IDocumentFactory* factory);
    static void UnregisterFactory(dbiplus::IDocumentFactory* factory);

  private:
    bool HasContent(const std::string& collection);
    void GetDirectoryContents(const VECFILEITEMS& items, VECFILEITEMS& files, std::set<std::string>& directories);
    void GetDatabaseContents(const std::string& url, VECFILEITEMS& files, std::set<std::string>& directories);
    void MergeFiles(VECFILEITEMS& oldFiles, VECFILEITEMS& newFiles, VECFILEITEMS& orphanedFiles);

    static dbiplus::CNoSqlDatabase m_db;
  };
}
