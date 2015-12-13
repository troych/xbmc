/*
 *      Copyright (C) 2015 Team XBMC
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

#include "ControllerTranslator.h"
#include "ControllerDefinitions.h"

using namespace GAME;

const char* CControllerTranslator::TranslateFeatureType(FEATURE type)
{
  switch (type)
  {
    case FEATURE::SCALAR:           return LAYOUT_XML_ELM_BUTTON;
    case FEATURE::ANALOG_STICK:     return LAYOUT_XML_ELM_ANALOG_STICK;
    case FEATURE::ACCELEROMETER:    return LAYOUT_XML_ELM_ACCELEROMETER;
    case FEATURE::KEY:              return LAYOUT_XML_ELM_KEY;
    case FEATURE::RELATIVE_POINTER: return LAYOUT_XML_ELM_RELATIVE_POINTER;
    case FEATURE::ABSOLUTE_POINTER: return LAYOUT_XML_ELM_ABSOLUTE_POINTER;
    case FEATURE::MOTOR:            return LAYOUT_XML_ELM_MOTOR;
    default:
      break;
  }
  return "";
}

FEATURE CControllerTranslator::TranslateFeatureType(const std::string& strType)
{
  if (strType == LAYOUT_XML_ELM_BUTTON)           return FEATURE::SCALAR;
  if (strType == LAYOUT_XML_ELM_ANALOG_STICK)     return FEATURE::ANALOG_STICK;
  if (strType == LAYOUT_XML_ELM_ACCELEROMETER)    return FEATURE::ACCELEROMETER;
  if (strType == LAYOUT_XML_ELM_KEY)              return FEATURE::KEY;
  if (strType == LAYOUT_XML_ELM_RELATIVE_POINTER) return FEATURE::RELATIVE_POINTER;
  if (strType == LAYOUT_XML_ELM_ABSOLUTE_POINTER) return FEATURE::ABSOLUTE_POINTER;
  if (strType == LAYOUT_XML_ELM_MOTOR)            return FEATURE::MOTOR;

  return FEATURE::UNKNOWN;
}

const char* CControllerTranslator::TranslateButtonType(JOYSTICK::INPUT type)
{
  switch (type)
  {
    case JOYSTICK::INPUT::DIGITAL: return "digital";
    case JOYSTICK::INPUT::ANALOG:  return "analog";
    default:
      break;
  }
  return "";
}

JOYSTICK::INPUT CControllerTranslator::TranslateButtonType(const std::string& strType)
{
  if (strType == "digital") return JOYSTICK::INPUT::DIGITAL;
  if (strType == "analog")  return JOYSTICK::INPUT::ANALOG;

  return JOYSTICK::INPUT::UNKNOWN;
}

const char* CControllerTranslator::TranslateGeometry(GEOMETRY geometry)
{
  switch (geometry)
  {
    case GEOMETRY::RECTANGLE: return "rectangle";
    case GEOMETRY::CIRCLE:    return "circle";
    default:
      break;
  }
  return "none";
}

GEOMETRY CControllerTranslator::TranslateGeometry(const std::string& strGeometry)
{
  if (strGeometry == "rectangle") return GEOMETRY::RECTANGLE;
  if (strGeometry == "circle")    return GEOMETRY::CIRCLE;

  return GEOMETRY::NONE;
}
