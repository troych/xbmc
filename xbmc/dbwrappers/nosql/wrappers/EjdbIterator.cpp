/*
 *      Copyright (C) 2015 Team Kodi
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

#include "EjdbIterator.h"
#include "BsonUtils.h"
#include "utils/Variant.h"

#include <ejdb/ejdb.h>

using namespace dbiplus;

CEjdbIterator::CEjdbIterator(void* results) :
  m_results(results),
  m_index(0),
  m_currentResult(Read())
{
}

CEjdbIterator::~CEjdbIterator(void)
{
  tclistdel(static_cast<TCLIST*>(m_results));
}

bool CEjdbIterator::IsValid(void) const
{
  return (int)m_index < TCLISTNUM(static_cast<TCLIST*>(m_results));
}

void CEjdbIterator::Next(void)
{
  m_index++;
  m_currentResult = Read();
}

CVariant CEjdbIterator::Read(void) const
{
  CVariant result;

  if (IsValid())
  {
    void* bsdata = TCLISTVALPTR(static_cast<TCLIST*>(m_results), m_index);
    bson_type type = BSON_NULL; // TODO
    bson_iterator* it = NULL; // TODO
    result = CBsonUtils::Read(type, it);
  }

  return result;
}
