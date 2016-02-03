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

#include "EventScanner.h"
#include "threads/SingleLock.h"
#include "threads/SystemClock.h"

#include <assert.h>

using namespace PERIPHERALS;
using namespace XbmcThreads;

#define DEFAULT_SCAN_FREQUENCY_HZ  60

CEventScanner::CEventScanner(IEventScannerCallback* callback) :
  CThread("PeripEventScanner"),
  m_callback(callback)
{
  assert(m_callback != nullptr);
}

void CEventScanner::Start(void)
{
  Create();
}

void CEventScanner::Stop(void)
{
  m_scanEvent.Set();
  StopThread(true);
}

EventFrequencyHandle CEventScanner::SetFrequency(float frequencyHz)
{
  CSingleLock lock(m_mutex);

  EventFrequencyHandle handle = EventFrequencyHandle(new CEventFrequencyHandle(frequencyHz, this));
  m_handles.push_back(handle);

  return handle;
}

void CEventScanner::Release(CEventFrequencyHandle* handle)
{
  CSingleLock lock(m_mutex);

  m_handles.erase(std::remove_if(m_handles.begin(), m_handles.end(),
    [handle](const EventFrequencyHandle& myHandle)
    {
      return handle == myHandle.get();
    }), m_handles.end());
}

void CEventScanner::Process(void)
{
  float nextScanMs = static_cast<float>(SystemClockMillis());

  while (!m_bStop)
  {
    m_scanEvent.Reset();

    m_callback->ProcessEvents();

    const float nowMs = static_cast<float>(SystemClockMillis());
    const float scanIntervalMs = GetScanIntervalMs();

    while (nextScanMs <= nowMs)
      nextScanMs += scanIntervalMs;

    unsigned int waitTimeMs = static_cast<unsigned int>(nextScanMs - nowMs);

    if (!m_bStop && waitTimeMs > 0)
      m_scanEvent.WaitMSec(waitTimeMs);
  }
}

float CEventScanner::GetFrequencyHz(void) const
{
  CSingleLock lock(m_mutex);

  float scanFrequencyHz = 0.0f;

  for (const EventFrequencyHandle& handle : m_handles)
  {
    if (handle->GetFrequencyHz() > scanFrequencyHz)
      scanFrequencyHz = handle->GetFrequencyHz();
  }

  if (scanFrequencyHz == 0.0f)
    scanFrequencyHz = DEFAULT_SCAN_FREQUENCY_HZ;

  return scanFrequencyHz;
}
