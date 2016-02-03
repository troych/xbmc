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

#include "EventScanFrequency.h"
#include "threads/CriticalSection.h"
#include "threads/Event.h"
#include "threads/Thread.h"

namespace PERIPHERALS
{
  class IEventScannerCallback
  {
  public:
    virtual ~IEventScannerCallback(void) { }

    virtual void ProcessEvents(void) = 0;
  };

  /*!
   * \brief Class to scan for peripheral events
   *
   * A default frequency of 60 Hz is used. This can be overridden by calling
   * SetFrequency(). The scanner will run at this new frequency until the
   * handle it returns has expired.
   *
   * If two instances hold handles from SetFrequency(), the one with the higher
   * frequency wins.
   */
  class CEventScanner : public IEventFrequencyCallback,
                        protected CThread
  {
  public:
    CEventScanner(IEventScannerCallback* callback);

    virtual ~CEventScanner(void) { }

    void Start(void);
    void Stop(void);

    EventFrequencyHandle SetFrequency(float frequencyHz);

    // implementation of IEventFrequencyCallback
    virtual void Release(CEventFrequencyHandle* handle);

  protected:
    // implementation of CThread
    virtual void Process(void) override;

  private:
    float GetFrequencyHz(void) const;
    float GetScanIntervalMs(void) const { return 1000.0f / GetFrequencyHz(); }

    IEventScannerCallback* const      m_callback;
    std::vector<EventFrequencyHandle> m_handles;
    CEvent                            m_scanEvent;
    CCriticalSection                  m_mutex;
  };
}
