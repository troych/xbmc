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

#include "ControllerLayout.h"
#include "ControllerDefinitions.h"
#include "utils/log.h"
#include "utils/XMLUtils.h"

#include <algorithm>
#include <sstream>

using namespace GAME;
using namespace JOYSTICK;

// --- FeatureTypeEqual --------------------------------------------------------

struct FeatureTypeEqual
{
  FeatureTypeEqual(FEATURE_TYPE type, INPUT_TYPE buttonType) : type(type), buttonType(buttonType) { }

  bool operator()(const CControllerFeature& feature) const
  {
    if (type == FEATURE_TYPE::UNKNOWN)
      return true; // Match all feature types

    if (type == FEATURE_TYPE::SCALAR && feature.Type() == FEATURE_TYPE::SCALAR)
    {
      if (buttonType == INPUT_TYPE::UNKNOWN)
        return true; // Match all button types

      return buttonType == feature.ButtonType();
    }

    return type == feature.Type();
  }

  const FEATURE_TYPE type;
  const INPUT_TYPE   buttonType;
};

// --- CControllerLayout ---------------------------------------------------

void CControllerLayout::Reset(void)
{
  m_label = 0;
  m_strImage.clear();
  m_strOverlay.clear();
  m_width = 0;
  m_height = 0;
  m_features.clear();
}

unsigned int CControllerLayout::FeatureCount(FEATURE_TYPE type       /* = FEATURE_TYPE::UNKNOWN */,
                                             INPUT_TYPE   buttonType /* = INPUT_TYPE::UNKNOWN */) const
{
  return std::count_if(m_features.begin(), m_features.end(), FeatureTypeEqual(type, buttonType));
}

bool CControllerLayout::Deserialize(const TiXmlElement* pElement, const CController* controller)
{
  Reset();

  if (!pElement)
    return false;

  // Label
  std::string strLabel = XMLUtils::GetAttribute(pElement, LAYOUT_XML_ATTR_LAYOUT_LABEL);
  if (strLabel.empty())
  {
    CLog::Log(LOGERROR, "<%s> tag has no \"%s\" attribute", LAYOUT_XML_ROOT, LAYOUT_XML_ATTR_LAYOUT_LABEL);
    return false;
  }
  std::istringstream(strLabel) >> m_label;

  // Image
  m_strImage = XMLUtils::GetAttribute(pElement, LAYOUT_XML_ATTR_LAYOUT_IMAGE);
  if (m_strImage.empty())
    CLog::Log(LOGDEBUG, "<%s> tag has no \"%s\" attribute", LAYOUT_XML_ROOT, LAYOUT_XML_ATTR_LAYOUT_IMAGE);

  // Overlay
  m_strOverlay = XMLUtils::GetAttribute(pElement, LAYOUT_XML_ATTR_LAYOUT_OVERLAY);
  if (m_strOverlay.empty())
    CLog::Log(LOGDEBUG, "<%s> tag has no \"%s\" attribute", LAYOUT_XML_ROOT, LAYOUT_XML_ATTR_LAYOUT_OVERLAY);

  // Width
  std::string strWidth = XMLUtils::GetAttribute(pElement, LAYOUT_XML_ATTR_LAYOUT_WIDTH);
  if (strWidth.empty())
    CLog::Log(LOGDEBUG, "<%s> tag has no \"%s\" attribute", LAYOUT_XML_ROOT, LAYOUT_XML_ATTR_LAYOUT_WIDTH);
  else
    std::istringstream(strWidth) >> m_width;

  // Height
  std::string strHeight = XMLUtils::GetAttribute(pElement, LAYOUT_XML_ATTR_LAYOUT_HEIGHT);
  if (strHeight.empty())
    CLog::Log(LOGDEBUG, "<%s> tag has no \"%s\" attribute", LAYOUT_XML_ROOT, LAYOUT_XML_ATTR_LAYOUT_HEIGHT);
  else
    std::istringstream(strHeight) >> m_height;

  // Features
  for (const TiXmlElement* pCategory = pElement->FirstChildElement(); pCategory != NULL; pCategory = pCategory->NextSiblingElement())
  {
    std::string strCategory = pElement->Value();

    // TODO: Something with category

    for (const TiXmlElement* pFeature = pCategory->FirstChildElement(); pFeature != NULL; pFeature = pFeature->NextSiblingElement())
    {
      CControllerFeature feature;

      if (!feature.Deserialize(pFeature, controller))
        return false;

      m_features.push_back(feature);
    }
  }

  return true;
}