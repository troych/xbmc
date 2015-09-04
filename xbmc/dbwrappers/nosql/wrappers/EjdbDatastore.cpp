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

#include "EjdbDatastore.h"
#include "BsonUtils.h"
#include "EjdbIterator.h"
#include "utils/Variant.h"

#include <ejdb/ejdb.h>

using namespace dbiplus;

#define DB_NAME  "content"

CEjdbDatastore::CEjdbDatastore(void) :
  m_db(NULL)
{
}

bool CEjdbDatastore::Initialize(void)
{
  Deinitialize();

  m_db = ejdbnew();
  if (!ejdbopen(m_db, DB_NAME, JBOWRITER | JBOCREAT | JBOTRUNC))
  {
    m_db = NULL;
    return false;
  }

  return true;
}

void CEjdbDatastore::Deinitialize(void)
{
  if (m_db)
  {
    ejdbclose(m_db);
    ejdbdel(m_db);
    m_db = nullptr;
  }
}

bool CEjdbDatastore::Put(const std::string& collection, const CVariant& document)
{
  if (!m_db)
    return false;

  if (!document.isObject())
    return false;

  // Get or create collection
  EJCOLL* coll = ejdbcreatecoll(m_db, collection.c_str(), NULL);

  bson bsrec;
  bson_oid_t oid;

  bson_init(&bsrec);

  bool success = true;
  for (CVariant::const_iterator_map it = document.begin_map(); it != document.end_map() && success; it++)
    success &= CBsonUtils::Write(&bsrec, it->first.c_str(), it->second);

  if (!success)
    return false;

  bson_finish(&bsrec);
  ejdbsavebson(coll, &bsrec, &oid);
  bson_destroy(&bsrec);
  return true;
}

DocumentIteratorPtr CEjdbDatastore::Get(const std::string& collection, const CVariant& query)
{
  DocumentIteratorPtr resultItr;

  if (!m_db)
    return resultItr;

  EJCOLL* coll = ejdbcreatecoll(m_db, collection.c_str(), NULL);

  bson bq1;
  bson_init_as_query(&bq1);

  bool success = true;
  if (query.isObject())
  {
    for (CVariant::const_iterator_map it = query.begin_map(); it != query.end_map() && success; it++)
      success &= CBsonUtils::Write(&bq1, it->first.c_str(), it->second);
  }

  bson_finish(&bq1);

  if (success)
  {
    EJQ* q1 = ejdbcreatequery(m_db, &bq1, NULL, 0, NULL);

    uint32_t count;
    TCLIST* results = ejdbqryexecute(coll, q1, &count, 0, NULL);

    if (results)
      resultItr = DocumentIteratorPtr(new CEjdbIterator(results));

    // Dispose query
    ejdbquerydel(q1);
  }

  bson_destroy(&bq1);

  return resultItr;
}

bool CEjdbDatastore::Delete(const std::string& collection, const CVariant& query)
{
  if (!m_db)
    return false;

  if (!query.isObject())
    return false;

  return false; // TODO
}
