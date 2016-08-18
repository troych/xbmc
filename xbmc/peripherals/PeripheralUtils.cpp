/*
 *      Copyright (C) 2014-2016 Team Kodi
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

#include "PeripheralUtils.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <iterator>

using namespace PERIPHERALS;

#define MAX_FILENAME_LENGTH  50

std::string CPeripheralUtils::SanitizeFileName(const std::string& deviceName)
{
  std::string sanitized;

  sanitized.reserve(deviceName.length());

  // Replace invalid characters with '_'
  std::transform(deviceName.begin(), deviceName.end(), std::back_inserter(sanitized),
    [](char c)
    {
      if (('a' <= c && c <= 'z') ||
          ('A' <= c && c <= 'Z') ||
          ('0' <= c && c <= '9') ||
          c == '-' ||
          c == '.' ||
          c == '_' ||
          c == '~')
      {
        return c;
      }
      return '_';
    });

  // Limit filename to a sane number of characters
  if (sanitized.length() > MAX_FILENAME_LENGTH)
    sanitized.erase(sanitized.begin() + MAX_FILENAME_LENGTH, sanitized.end());

  // Trim leading and trailing underscores
  sanitized = StringUtils::Trim(sanitized, "_");

  return sanitized;
}
