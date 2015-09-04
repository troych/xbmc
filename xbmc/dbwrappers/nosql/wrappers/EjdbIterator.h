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

#include "dbwrappers/nosql/IDocumentIterator.h"
#include "utils/Variant.h"

namespace dbiplus
{
  class CEjdbIterator : public IDocumentIterator
  {
  public:
    /*!
     * \brief Create EJDB document store
     * \param results pointer to a TCLIST struct
     */
    CEjdbIterator(void* results);

    virtual ~CEjdbIterator(void);

    // implementation of IDocumentIterator
    virtual CVariant& GetRecord(void) override { return m_currentResult; }
    virtual bool IsValid(void) const override;
    virtual void Next(void) override;

  private:
    CVariant Read(void) const;

    void*        m_results; // struct TCLIST
    unsigned int m_index;
    CVariant     m_currentResult;
  };
}
