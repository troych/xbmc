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

#include "Directory.h"
#include "utils/Variant.h"

#include <algorithm>
#include <utility>

using namespace XFILE;
using namespace XFILE::DB;

// --- GetVariantArray() -------------------------------------------------------

namespace
{
  template <typename T>
  static void GetVariantArray(const T& items, CVariant& value)
  {
    for (typename T::const_iterator it = items.begin(); it != items.end(); ++it)
      value.push_back(*it);
  }
}

// --- CDirectory --------------------------------------------------------------

void CDirectory::Reset(void)
{
  m_url.clear();
  m_bEnabled = true;
  m_fileIds.clear();
  m_directoryNames.clear();
}

void CDirectory::Serialize(CVariant& value) const
{
  if (!m_url.empty())
    value["url"] = m_url;
  if (!m_bEnabled) // Only store if changed from the default
    value["enabled"] = m_bEnabled;
  if (!m_fileIds.empty())
    GetVariantArray(m_fileIds, value["file_ids"]);
  if (!m_directoryNames.empty())
    GetVariantArray(m_directoryNames, value["directory_names"]);
}

void CDirectory::Deserialize(const CVariant& value)
{
  Reset();
  m_url = value["url"].asString();
  m_bEnabled = value["enabled"].asBoolean();
  const CVariant& fileIds = value["file_ids"];
  if (fileIds.isArray())
  {
    for (CVariant::const_iterator_array it = fileIds.begin_array(); it != fileIds.end_array(); ++it)
    {
      if (it->isInteger())
        m_fileIds.push_back(it->asInteger());
    }
  }
  const CVariant& directoryNames = value["directory_names"];
  if (directoryNames.isArray())
  {
    for (CVariant::const_iterator_array it = directoryNames.begin_array(); it != directoryNames.end_array(); ++it)
    {
      if (it->isString())
        m_directoryNames.insert(it->asString());
    }
  }
}

void CDirectory::Deserialize(CVariant&& value)
{
  Reset();
  m_url = std::move(value["url"].asString());
  m_bEnabled = value["enabled"].asBoolean();
  const CVariant& fileIds = value["file_ids"];
  if (fileIds.isArray())
  {
    for (CVariant::const_iterator_array it = fileIds.begin_array(); it != fileIds.end_array(); ++it)
    {
      if (it->isInteger())
        m_fileIds.push_back(it->asInteger());
    }
  }
  const CVariant& directoryNames = value["directory_names"];
  if (directoryNames.isArray())
  {
    for (CVariant::const_iterator_array it = directoryNames.begin_array(); it != directoryNames.end_array(); ++it)
    {
      if (it->isString())
        m_directoryNames.insert(std::move(it->asString()));
    }
  }
}

void CDirectory::GetIntArray(const CVariant& varInts, std::vector<int64_t>& vecInts)
{
  if (varInts.isArray())
  {
    for (CVariant::const_iterator_array it = varInts.begin_array(); it != varInts.end_array(); ++it)
    {
      if (it->isInteger())
        vecInts.push_back(it->asInteger());
    }
  }

  // NoSQL databases are typically optimized for in-order traversal
  std::sort(vecInts.begin(), vecInts.end());
}

void CDirectory::GetStringArray(const CVariant& varStrings, std::set<std::string>& vecStrings)
{
  if (varStrings.isArray())
  {
    for (CVariant::const_iterator_array it = varStrings.begin_array(); it != varStrings.end_array(); ++it)
    {
      if (it->isString())
        vecStrings.insert(it->asString());
    }
  }
}
