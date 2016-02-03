/*
 *      Copyright (C) 2016 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include <memory>

namespace PERIPHERALS
{
  class CEventFrequencyHandle;
  typedef std::shared_ptr<CEventFrequencyHandle> EventFrequencyHandle;

  /*!
   * \brief Callback implemented by event scanner
   */
  class IEventFrequencyCallback
  {
  public:
    virtual ~IEventFrequencyCallback(void) { }

    /*!
     * \brief Release the specified handle
     */
    virtual void Release(CEventFrequencyHandle* handle) = 0;
  };

  /*!
   * \brief Handle returned by the event scanner when a scan frequency is requested
   */
  class CEventFrequencyHandle
  {
  public:
    CEventFrequencyHandle(float frequencyHz, IEventFrequencyCallback* callback);

    ~CEventFrequencyHandle(void) { Release(); }

    /*!
     * \brief Get the frequency this handle represents
     */
    float GetFrequencyHz(void) const { return m_frequencyHz; }

    /*!
     * \brief Release the handle
     *
     * Call this to release the handle early. Otherwise, the handle is
     * automatically released when the shared pointer's ref count drops
     * to zero.
     */
    void Release(void);

  private:
    const float                    m_frequencyHz;
    IEventFrequencyCallback* const m_callback;
  };
}
