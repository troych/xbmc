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

#include "NoSqlIterator.h"
#include "IDocument.h"
#include "IDocumentFactory.h"
#include "IDocumentIterator.h"
#include "utils/Variant.h"

#include <utility>

using namespace dbiplus;

CNoSqlIterator::CNoSqlIterator(const DocumentIteratorPtr& it, const std::string& collection, IDocumentFactory* factory) :
  m_it(it),
  m_collection(collection),
  m_factory(factory)
{
  m_bIsValid = m_it->IsValid();
}

DocumentPtr CNoSqlIterator::GetRecord(bool bDestructive /* = false */)
{
  DocumentPtr document;
  
  if (m_bIsValid)
  {
    document = m_factory->CreateDocument(m_collection);
    if (document)
    {
      if (bDestructive)
      {
        document->Deserialize(std::move(m_it->GetRecord()));
        m_bIsValid = false;
      }
      else
      {
        document->Deserialize(m_it->GetRecord());
      }
    }
    else
    {
      // Failed to create document, mark as invalid
      m_bIsValid = false;
    }
  }

  return document;
}

void CNoSqlIterator::Next(void)
{
  m_it->Next();
  m_bIsValid = m_it->IsValid();
}
