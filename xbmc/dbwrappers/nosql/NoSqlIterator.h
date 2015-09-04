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

namespace dbiplus
{
  class IDocumentFactory;

  class CNoSqlIterator
  {
  public:
    CNoSqlIterator(const DocumentIteratorPtr& it, const std::string& collectin, IDocumentFactory* factory);

    DocumentPtr GetRecord(bool bDestructive = false);
    
    //! \copydoc IDocumentIterator::IsValid
    bool IsValid(void) const { return m_bIsValid; }

    //! \copydoc IDocumentIterator::Delete
    void Next(void);

  private:
    const DocumentIteratorPtr m_it;
    const std::string         m_collection;
    IDocumentFactory* const   m_factory;
    bool                      m_bIsValid;
  };
}
