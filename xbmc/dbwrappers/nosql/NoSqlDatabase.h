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
#pragma once

#include "IDocumentFactory.h"
#include "NoSqlTypes.h"

#include <string>
#include <vector>

class CVariant;

namespace dbiplus
{
  class IDocumentStore;

  class CNoSqlDatabase : public IDocumentFactory
  {
  public:
    CNoSqlDatabase(void);
    ~CNoSqlDatabase(void);

    // implementation of IDocumentFactory
    virtual DocumentPtr CreateDocument(const std::string& collection) const override;
    virtual bool HasCollection(const std::string& collection) const override;

    void RegisterFactory(IDocumentFactory* factory);
    void UnregisterFactory(IDocumentFactory* factory);

    bool Put(const std::string& collection, const IDocument* document);

    NoSqlIteratorPtr GetDocuments(const std::string& collection);
    NoSqlIteratorPtr GetDocuments(const std::string& collection, const CVariant& query);
    DocumentPtr GetDocumentByID(const std::string& collection, int64_t identifier);
    DocumentPtr GetDocumentByProperty(const std::string& collection, const std::string& strProperty, const CVariant& value);

  private:
    //! \copydoc IDocumentStore::Put
    bool Put(const std::string& collection, const CVariant& document);

    //! \copydoc IDocumentStore::Get
    DocumentIteratorPtr Get(const std::string& collection, const CVariant& query);

    //! \copydoc IDocumentStore::Delete
    bool Delete(const std::string& collection, const CVariant& query);

    IDocumentStore*                m_db;
    bool                           m_bInitialized;
    std::vector<IDocumentFactory*> m_factories;
  };
}
