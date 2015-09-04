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

#include "ContentDatabaseFilter.h"
#include "utils/JSONVariantParser.h"

using namespace XFILE;

CContentDatabaseFilter::CContentDatabaseFilter(const std::string& type) : 
  m_type(type)
{
}

CContentDatabaseFilter::CContentDatabaseFilter(const std::string& type, const CVariant& query) : 
  m_type(type),
  m_query(query)
{
}

CContentDatabaseFilter::CContentDatabaseFilter(const std::string& type, const std::string& strJsonQuery) : 
  m_type(type),
  m_query(CJSONVariantParser::Parse(reinterpret_cast<const unsigned char*>(strJsonQuery.c_str()), strJsonQuery.length()))
{
}
