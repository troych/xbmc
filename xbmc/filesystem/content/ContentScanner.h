#pragma once
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
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

#include "threads/CriticalSection.h"
#include "threads/Thread.h"
#include "FileItem.h"

#include <string>

namespace XFILE
{
  class CContentScanner : protected CThread
  {
  private:
    CContentScanner(void);

  public:
    static CContentScanner& Get(void);

    ~CContentScanner(void);

    void Scan(const std::string& strPath);

  protected:
    void Process(void);

  private:
    void DoScan(const std::string& strPath);

    std::string       m_strPath;
    CEvent            m_scanEvent;
    CCriticalSection  m_mutex;
  };
}
