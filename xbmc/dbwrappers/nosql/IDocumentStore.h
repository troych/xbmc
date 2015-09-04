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

#include "NoSqlTypes.h"

#include <string>

class CVariant;

namespace dbiplus
{
  class IDocumentStore
  {
  public:
    virtual ~IDocumentStore(void) { }

    /*!
     * \brief Initialize the document store
     */
    virtual bool Initialize(void) = 0;

    /*!
     * \brief Deinitialize the document store
     */
    virtual void Deinitialize(void) = 0;

    /*!
     * \brief Insert a document into a collection
     *
     * If the document does not contain the _id field, the field is
     * automatically set to a generated ObjectId.
     */
    virtual bool Put(const std::string& collection, const CVariant& document) = 0;

    /*!
     * \brief Select a document from a collection
     *
     * TODO: Document query synctax
     */
    virtual DocumentIteratorPtr Get(const std::string& collection, const CVariant& query) = 0;

    /*!
     * \brief Remove one or more documents
     */
    virtual bool Delete(const std::string& collection, const CVariant& query) = 0;
  };
}
