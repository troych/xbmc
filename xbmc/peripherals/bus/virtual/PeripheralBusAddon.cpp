/*
 *      Copyright (C) 2014-2015 Team XBMC
 *      http://xbmc.org
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

#include "PeripheralBusAddon.h"
#include "addons/Addon.h"
#include "addons/AddonManager.h"
#include "peripherals/Peripherals.h"
#include "peripherals/addons/PeripheralAddon.h"
#include "peripherals/devices/PeripheralJoystick.h"
#include "threads/SingleLock.h"
#include "utils/log.h"

#include <algorithm>
#include <memory>

using namespace ADDON;
using namespace PERIPHERALS;

CPeripheralBusAddon::CPeripheralBusAddon(CPeripherals *manager) :
    CPeripheralBus("PeripBusAddon", manager, PERIPHERAL_BUS_ADDON)
{
}

bool CPeripheralBusAddon::GetAddon(const std::string &strId, AddonPtr &addon) const
{
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator it = m_addons.begin(); it != m_addons.end(); ++it)
  {
    if ((*it)->ID() == strId)
    {
      addon = *it;
      return true;
    }
  }
  for (PeripheralAddonVector::const_iterator it = m_failedAddons.begin(); it != m_failedAddons.end(); ++it)
  {
    if ((*it)->ID() == strId)
    {
      addon = *it;
      return true;
    }
  }
  return false;
}

bool CPeripheralBusAddon::GetAddonWithButtonMap(const CPeripheral* device, PeripheralAddonPtr &addon) const
{
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator it = m_addons.begin(); it != m_addons.end(); ++it)
  {
    if ((*it)->HasFeature(FEATURE_JOYSTICK)) // TODO: Check for button map support
    {
      addon = *it;
      return true;
    }
  }
  return false;
}

unsigned int CPeripheralBusAddon::GetAddonCount(void) const
{
  CSingleLock lock(m_critSection);
  return m_addons.size();
}

bool CPeripheralBusAddon::PerformDeviceScan(PeripheralScanResults &results)
{
  UpdateAddons();

  PeripheralAddonVector addons;
  {
    CSingleLock lock(m_critSection);
    addons = m_addons;
  }
  
  for (PeripheralAddonVector::const_iterator it = addons.begin(); it != addons.end(); ++it)
    (*it)->PerformDeviceScan(results);

  // Scan during bus initialization must return true or bus gets deleted
  return true;
}

bool CPeripheralBusAddon::InitializeProperties(CPeripheral* peripheral)
{
  bool bSuccess = false;

  PeripheralAddonPtr addon;
  unsigned int index;

  if (SplitLocation(peripheral->Location(), addon, index))
  {
    switch (peripheral->Type())
    {
      case PERIPHERAL_JOYSTICK:
        bSuccess = addon->GetJoystickProperties(index, *static_cast<CPeripheralJoystick*>(peripheral));
        break;

      default:
        break;
    }
  }

  return bSuccess;
}

void CPeripheralBusAddon::ProcessEvents(void)
{
  PeripheralAddonVector addons;

  {
    CSingleLock lock(m_critSection);
    addons = m_addons;
  }

  for (PeripheralAddonVector::const_iterator itAddon = addons.begin(); itAddon != addons.end(); ++itAddon)
    (*itAddon)->ProcessEvents();
}

void CPeripheralBusAddon::UnregisterRemovedDevices(const PeripheralScanResults &results)
{
  CSingleLock lock(m_critSection);

  std::vector<CPeripheral*> removedPeripherals;

  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    (*itAddon)->UnregisterRemovedDevices(results, removedPeripherals);

  for (unsigned int iDevicePtr = 0; iDevicePtr < removedPeripherals.size(); iDevicePtr++)
  {
    CPeripheral *peripheral = removedPeripherals.at(iDevicePtr);
    m_manager->OnDeviceDeleted(*this, *peripheral);
    delete peripheral;
  }
}

void CPeripheralBusAddon::Register(CPeripheral* peripheral)
{
  if (!peripheral)
    return;

  PeripheralAddonPtr addon;
  unsigned int       peripheralIndex;

  CSingleLock lock(m_critSection);

  if (SplitLocation(peripheral->Location(), addon, peripheralIndex))
  {
    if (addon->Register(peripheralIndex, peripheral))
      m_manager->OnDeviceAdded(*this, *peripheral);
  }
}

void CPeripheralBusAddon::GetFeatures(std::vector<PeripheralFeature> &features) const
{
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    (*itAddon)->GetFeatures(features);
}

bool CPeripheralBusAddon::HasFeature(const PeripheralFeature feature) const
{
  bool bReturn(false);
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    bReturn = bReturn || (*itAddon)->HasFeature(feature);
  return bReturn;
}

CPeripheral *CPeripheralBusAddon::GetPeripheral(const std::string &strLocation) const
{
  CPeripheral*       peripheral(NULL);
  PeripheralAddonPtr addon;
  unsigned int       peripheralIndex;

  CSingleLock lock(m_critSection);

  if (SplitLocation(strLocation, addon, peripheralIndex))
    peripheral = addon->GetPeripheral(peripheralIndex);

  return peripheral;
}

CPeripheral *CPeripheralBusAddon::GetByPath(const std::string &strPath) const
{
  CSingleLock lock(m_critSection);

  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
  {
    CPeripheral* peripheral = (*itAddon)->GetByPath(strPath);
    if (peripheral)
      return peripheral;
  }

  return NULL;
}

int CPeripheralBusAddon::GetPeripheralsWithFeature(std::vector<CPeripheral *> &results, const PeripheralFeature feature) const
{
  int iReturn(0);
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    iReturn += (*itAddon)->GetPeripheralsWithFeature(results, feature);
  return iReturn;
}

size_t CPeripheralBusAddon::GetNumberOfPeripherals(void) const
{
  size_t iReturn(0);
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    iReturn += (*itAddon)->GetNumberOfPeripherals();
  return iReturn;
}

size_t CPeripheralBusAddon::GetNumberOfPeripheralsWithId(const int iVendorId, const int iProductId) const
{
  size_t iReturn(0);
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    iReturn += (*itAddon)->GetNumberOfPeripheralsWithId(iVendorId, iProductId);
  return iReturn;
}

void CPeripheralBusAddon::GetDirectory(const std::string &strPath, CFileItemList &items) const
{
  CSingleLock lock(m_critSection);
  for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    (*itAddon)->GetDirectory(strPath, items);
}

bool CPeripheralBusAddon::SplitLocation(const std::string& strLocation, PeripheralAddonPtr& addon, unsigned int& peripheralIndex) const
{
  std::vector<std::string> parts = StringUtils::Split(strLocation, "/");
  if (parts.size() == 2)
  {
    addon.reset();

    CSingleLock lock(m_critSection);

    const std::string& strAddonId = parts[0];
    for (PeripheralAddonVector::const_iterator itAddon = m_addons.begin(); itAddon != m_addons.end(); ++itAddon)
    {
      if ((*itAddon)->ID() == strAddonId)
      {
        addon = *itAddon;
        break;
      }
    }

    if (addon)
    {
      const char* strJoystickIndex = parts[1].c_str();
      char* p = NULL;
      peripheralIndex = strtol(strJoystickIndex, &p, 10);
      if (strJoystickIndex != p)
        return true;
    }
  }
  return false;
}

void CPeripheralBusAddon::UpdateAddons(void)
{
  PeripheralAddonVector removedAddons;
  PeripheralAddonVector newAddons;

  VECADDONS addons;
  CAddonMgr::Get().GetAddons(ADDON_PERIPHERALDLL, addons, true);

  {
    CSingleLock lock(m_critSection);
    
    // Search for removed add-ons
    for (PeripheralAddonVector::const_iterator it = m_addons.begin(); it != m_addons.end(); ++it)
    {
      const bool bRemoved = (std::find(addons.begin(), addons.end(),
        std::static_pointer_cast<CAddon>(*it)) == addons.end());

      if (bRemoved)
        removedAddons.push_back(*it);
    }

    // Search for new add-ons
    for (VECADDONS::const_iterator it = addons.begin(); it != addons.end(); ++it)
    {
      PeripheralAddonPtr addon = std::dynamic_pointer_cast<CPeripheralAddon>(*it);
      if (!addon)
        continue;

      // If add-on failed to load, skip it
      if (std::find(m_failedAddons.begin(), m_failedAddons.end(), addon) != m_failedAddons.end())
        continue;

      // If add-on has already been created, skip it
      if (std::find(m_addons.begin(), m_addons.end(), addon) != m_addons.end())
        continue;

      newAddons.push_back(addon);
    }

    // Update m_addons
    for (PeripheralAddonVector::const_iterator it = removedAddons.begin(); it != removedAddons.end(); ++it)
      m_addons.erase(std::remove(m_addons.begin(), m_addons.end(), *it), m_addons.end());
    for (PeripheralAddonVector::const_iterator it = newAddons.begin(); it != newAddons.end(); ++it)
      m_addons.push_back(*it);
  }

  // Destroy removed add-ons
  for (PeripheralAddonVector::const_iterator it = removedAddons.begin(); it != removedAddons.end(); ++it)
    (*it)->Destroy();

  // Create new add-ons
  for (PeripheralAddonVector::const_iterator it = newAddons.begin(); it != newAddons.end(); ++it)
  {
    if ((*it)->CreateAddon() != ADDON_STATUS_OK)
    {
      CSingleLock lock(m_critSection);
      m_addons.erase(std::remove(m_addons.begin(), m_addons.end(), *it), m_addons.end());
      m_failedAddons.push_back(*it);
    }
  }
}