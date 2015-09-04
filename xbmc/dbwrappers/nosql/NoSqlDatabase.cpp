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

#include "NoSqlDatabase.h"
#include "IDocument.h"
#include "IDocumentIterator.h"
#include "NoSqlIterator.h"
#include "dbwrappers/nosql/wrappers/EjdbDatastore.h"
#include "utils/Variant.h"

#include <algorithm>

using namespace dbiplus;

#define DOCUMENT_ID  "_id"

CNoSqlDatabase::CNoSqlDatabase(void) :
  m_bInitialized(false)
{
  m_db = new CEjdbDatastore;
}

CNoSqlDatabase::~CNoSqlDatabase(void)
{
  if (m_bInitialized)
    m_db->Deinitialize();

  delete m_db;
}

DocumentPtr CNoSqlDatabase::CreateDocument(const std::string& collection) const
{
  DocumentPtr document;
  for (std::vector<IDocumentFactory*>::const_iterator it = m_factories.begin(); it != m_factories.end(); ++it)
  {
    document = (*it)->CreateDocument(collection);
    if (document)
      break;
  }
  return document;
}

bool CNoSqlDatabase::HasCollection(const std::string& collection) const
{
  for (std::vector<IDocumentFactory*>::const_iterator it = m_factories.begin(); it != m_factories.end(); ++it)
  {
    if ((*it)->HasCollection(collection))
      return true;
  }
  return false;
}

void CNoSqlDatabase::RegisterFactory(IDocumentFactory* factory)
{
  m_factories.push_back(factory);
}

void CNoSqlDatabase::UnregisterFactory(IDocumentFactory* factory)
{
  m_factories.erase(std::remove(m_factories.begin(), m_factories.end(), factory), m_factories.end());
}

bool CNoSqlDatabase::Put(const std::string& collection, const IDocument* document)
{
  CVariant variant;
  document->Serialize(variant);
  return Put(collection, variant);
}

NoSqlIteratorPtr CNoSqlDatabase::GetDocuments(const std::string& collection)
{
  CVariant dummy;
  return GetDocuments(collection, dummy);
}

NoSqlIteratorPtr CNoSqlDatabase::GetDocuments(const std::string& collection, const CVariant& query)
{
  NoSqlIteratorPtr itWrapper;

  DocumentIteratorPtr it = Get(collection, query);
  if (it)
    itWrapper = NoSqlIteratorPtr(new CNoSqlIterator(it, collection, this));

  return itWrapper;
}

DocumentPtr CNoSqlDatabase::GetDocumentByID(const std::string& collection, int64_t identifier)
{
  return GetDocumentByProperty(collection, DOCUMENT_ID, identifier);
}

DocumentPtr CNoSqlDatabase::GetDocumentByProperty(const std::string& collection, const std::string& strProperty, const CVariant& value)
{
  DocumentPtr document;

  CVariant query;
  query[strProperty] = value;
  DocumentIteratorPtr it = Get(collection, query);
  if (it && it->IsValid())
  {
    document = CreateDocument(collection);
    if (document)
      document->Deserialize(std::move(it->GetRecord()));
  }

  return document;
}

bool CNoSqlDatabase::Put(const std::string& collection, const CVariant& document)
{
  if (!m_bInitialized)
  {
    m_bInitialized = true;
    m_db->Initialize();
  }

  return m_db->Put(collection, document);
}

DocumentIteratorPtr CNoSqlDatabase::Get(const std::string& collection, const CVariant& query)
{
  if (!m_bInitialized)
  {
    m_bInitialized = true;
    m_db->Initialize();
  }

  return m_db->Get(collection, query);
}

bool CNoSqlDatabase::Delete(const std::string& collection, const CVariant& query)
{
  if (!m_bInitialized)
  {
    m_bInitialized = true;
    m_db->Initialize();
  }

  return m_db->Delete(collection, query);
}
