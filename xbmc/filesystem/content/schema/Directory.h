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

#include "dbwrappers/nosql/IDocument.h"

#include <memory> // TODO: Move to a types file
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

class CVariant;

namespace XFILE
{
  namespace DB
  {
    // TODO: Move to a types file
    class CDirectory;
    typedef std::shared_ptr<CDirectory> DirectoryPtr;

    class CDirectory : public dbiplus::IDocument
    {
    public:
      CDirectory(void) { Reset(); }

      void Reset(void);

      // implementation of IDocument
      virtual void Serialize(CVariant& value) const override;
      virtual void Deserialize(const CVariant& value) override;
      virtual void Deserialize(CVariant&& value) override;

      const std::string&           URL(void) const            { return m_url; }
      bool                         IsEnabled(void) const      { return m_bEnabled; }
      const std::vector<int64_t>&  FileIDs(void) const        { return m_fileIds; }
      const std::set<std::string>& DirectoryNames(void) const { return m_directoryNames; }

      void                   SetURL(const std::string& url) { m_url = url; }
      void                   SetEnabled(bool bEnabled)      { m_bEnabled = bEnabled; }
      std::vector<int64_t>&  FileIDs(void)                  { return m_fileIds; }
      std::set<std::string>& DirectoryNames(void)           { return m_directoryNames; }

    private:
      // Helper functions
      static void GetIntArray(const CVariant& varInts, std::vector<int64_t>& vecInts);
      static void GetStringArray(const CVariant& varStrings, std::set<std::string>& vecStrings);

      std::string           m_url;
      bool                  m_bEnabled;
      std::vector<int64_t>  m_fileIds;
      std::set<std::string> m_directoryNames;
    };
  }
}
