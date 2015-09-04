/*
 *      Copyright (C) 2012-2015 Team XBMC
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

#include "GameInfoTag.h"
#include "games/GameDefinitions.h"
#include "games/GameTranslator.h"
#include "utils/Variant.h"

#include <algorithm>
#include <utility>

using namespace GAME;

void CGameInfoTag::Reset()
{
  m_id = -1;
  m_strTitle.clear();
  m_publisher.clear();
  m_developer.clear();
  m_strMediaType.clear();
  m_playerCount = 0;
  m_bCoop = false;
  m_releaseDate.Reset();
  m_addDate.Reset();
  m_rating = 0;
  m_esbr = ESBR_UNKNOWN;
  m_editions.clear();
  m_genres.clear();
  m_platforms.clear();
  m_series.clear();
  m_artwork.clear();
}

CGameInfoTag& CGameInfoTag::Merge(const CGameInfoTag& other)
{
  if (other.m_id >= 0)
    m_id = other.m_id;
  if (!other.m_strTitle.empty())
    m_strTitle = other.m_strTitle;
  if (!other.m_publisher.empty())
    m_publisher = other.m_publisher;
  if (!other.m_developer.empty())
    m_developer = other.m_developer;
  if (!other.m_strMediaType.empty())
    m_strMediaType = other.m_strMediaType;
  if (other.m_playerCount != 0)
    m_playerCount = other.m_playerCount;
  if (other.m_bCoop)
    m_bCoop = other.m_bCoop;
  if (other.m_releaseDate.IsValid())
    m_releaseDate = other.m_releaseDate;
  if (other.m_addDate.IsValid())
    m_addDate = other.m_addDate;
  if (other.m_rating != 0)
    m_rating = other.m_rating;
  if (other.m_esbr != ESBR_UNKNOWN)
    m_esbr = other.m_esbr;
  if (!other.m_editions.empty())
    m_editions = other.m_editions;
  if (!other.m_genres.empty())
    m_genres = other.m_genres;
  if (!other.m_platforms.empty())
    m_platforms = other.m_platforms;
  if (!other.m_series.empty())
    m_series = other.m_series;
  if (!other.m_artwork.empty())
    m_artwork = other.m_artwork;
  return *this;
}

const CGameInfoTag& CGameInfoTag::operator=(const CGameInfoTag& rhs)
{
  if (this != &rhs)
  {
    m_id           = rhs.m_id;
    m_strTitle     = rhs.m_strTitle;
    m_publisher    = rhs.m_publisher;
    m_developer    = rhs.m_developer;
    m_strMediaType = rhs.m_strMediaType;
    m_playerCount  = rhs.m_playerCount;
    m_bCoop        = rhs.m_bCoop;
    m_releaseDate  = rhs.m_releaseDate;
    m_addDate      = rhs.m_addDate;
    m_rating       = rhs.m_rating;
    m_esbr         = rhs.m_esbr;
    m_editions     = rhs.m_editions;
    m_genres       = rhs.m_genres;
    m_platforms    = rhs.m_platforms;
    m_series       = rhs.m_series;
    m_artwork      = rhs.m_artwork;
  }
  return *this;
}

void CGameInfoTag::Serialize(CVariant& value) const
{
  if (m_id >= 0)               value[GAME_ID] = m_id;
  if (!m_strTitle.empty())     value[GAME_TITLE] = m_strTitle;
  if (!m_publisher.empty())    value[GAME_PUBLISHER] = m_publisher;
  if (!m_developer.empty())    value[GAME_DEVELOPER] = m_developer;
  if (!m_strMediaType.empty()) value[GAME_MEDIA_TYPE] = m_strMediaType;
  if (m_playerCount != 0)      value[GAME_PLAYER_COUNT] = static_cast<int>(m_playerCount);
  if (m_bCoop)                 value[GAME_COOP] = m_bCoop;
  if (m_releaseDate.IsValid()) value[GAME_RELEASE_DATE] = m_releaseDate.GetAsDBDate();
  if (m_addDate.IsValid())     value[GAME_ADD_DATE] = m_addDate.GetAsDBDate();
  if (m_rating != 0)           value[GAME_RATING] = static_cast<int>(m_rating);
  if (m_esbr != ESBR_UNKNOWN)  value[GAME_ESBR] = static_cast<int>(m_esbr);
  for (const auto& edition : m_editions)
  {
    CVariant subValue;
    subValue[GAME_EDITION_FLAGS]     = edition.flags;
    subValue[GAME_EDITION_COUNTRIES] = edition.countries;
    subValue[GAME_EDITION_PATH]      = edition.path;
    subValue[GAME_EDITION_CRC]       = static_cast<int>(edition.crc);
    subValue[GAME_EDITION_PROVIDER]  = edition.provider;
    value[GAME_EDITIONS].push_back(subValue);
  }
  if (!m_genres.empty())       value[GAME_GENRES]    = m_genres;
  if (!m_platforms.empty())    value[GAME_PLATFORMS] = m_platforms;
  if (!m_series.empty())       value[GAME_SERIES]    = m_series;
  for (const auto& artwork : m_artwork)
  {
    CVariant subValue;
    subValue[GAME_ARTWORK_TYPE] = static_cast<int>(artwork.type);
    subValue[GAME_ARTWORK_PATH] = artwork.path;
    value[GAME_ARTWORK].push_back(subValue);
  }
}

void CGameInfoTag::Deserialize(const CVariant& value)
{
  Reset();
  m_id = value[GAME_ID].asInteger();
  m_strTitle = value[GAME_TITLE].asString();
  m_publisher = value[GAME_PUBLISHER].asString();
  m_developer = value[GAME_DEVELOPER].asString();
  m_strMediaType = value[GAME_MEDIA_TYPE].asString();
  m_playerCount = static_cast<unsigned int>(value[GAME_PLAYER_COUNT].asInteger());
  m_bCoop = value[GAME_COOP].asBoolean();
  m_releaseDate.SetFromDBDate(value[GAME_RELEASE_DATE].asString());
  m_addDate.SetFromDBDate(value[GAME_ADD_DATE].asString());
  m_rating = static_cast<unsigned int>(value[GAME_RATING].asInteger());
  m_esbr = static_cast<EsbrRating>(value[GAME_ESBR].asInteger());
  if (value[GAME_RATING].isArray())
  {
    for (auto it = value[GAME_RATING].begin_array(); it != value[GAME_RATING].end_array(); ++it)
    {
      const CVariant& subvalue = *it;
      if (subvalue.isObject())
      {
        GameEdition edition =
          {
            ROM_NO_FLAGS,
            COUNTRY_UNKNOWN,
            "",
            0,
            "",
          };
        if (subvalue[GAME_EDITION_FLAGS].isInteger())     edition.flags = static_cast<GameFlag>(subvalue[GAME_EDITION_FLAGS].asInteger());
        if (subvalue[GAME_EDITION_COUNTRIES].isInteger()) edition.countries = static_cast<CountryCode>(subvalue[GAME_EDITION_COUNTRIES].asInteger());
        if (subvalue[GAME_EDITION_PATH].isString())       edition.path = subvalue[GAME_EDITION_PATH].asString();
        if (subvalue[GAME_EDITION_CRC].isInteger())       edition.crc = static_cast<uint32_t>(subvalue[GAME_EDITION_CRC].asInteger());
        if (subvalue[GAME_EDITION_PROVIDER].isString())   edition.provider = subvalue[GAME_EDITION_PROVIDER].asString();
        m_editions.push_back(edition);
      }
    }
  }
  if (value[GAME_GENRES].isArray())
  {
    for (auto it = value[GAME_GENRES].begin_array(); it != value[GAME_GENRES].end_array(); ++it)
      if (it->isString()) m_genres.push_back(it->asString());
  }
  if (value[GAME_PLATFORMS].isArray())
  {
    for (auto it = value[GAME_PLATFORMS].begin_array(); it != value[GAME_PLATFORMS].end_array(); ++it)
      if (it->isString()) m_platforms.push_back(it->asString());
  }
  if (value[GAME_SERIES].isArray())
  {
    for (auto it = value[GAME_SERIES].begin_array(); it != value[GAME_SERIES].end_array(); ++it)
      if (it->isString()) m_series.push_back(it->asString());
  }
  if (value[GAME_ARTWORK].isArray())
  {
    for (auto it = value[GAME_ARTWORK].begin_array(); it != value[GAME_ARTWORK].end_array(); ++it)
    {
      const CVariant& subvalue = *it;
      if (subvalue.isObject())
      {
        GameArtwork artwork =
          {
            ARTWORK_UNKNOWN,
            "",
          };
        if (subvalue[GAME_ARTWORK_TYPE].isInteger()) artwork.type = static_cast<GameArtworkType>(subvalue[GAME_ARTWORK_TYPE].asInteger());
        if (subvalue[GAME_ARTWORK_PATH].isString())  artwork.path = subvalue[GAME_EDITION_PATH].asString();
        m_artwork.push_back(artwork);
      }
    }
  }
}

void CGameInfoTag::Deserialize(CVariant&& value)
{
  Reset();
  m_id = value[GAME_ID].asInteger();
  m_strTitle = std::move(value[GAME_TITLE].asString());
  m_publisher = std::move(value[GAME_PUBLISHER].asString());
  m_developer = std::move(value[GAME_DEVELOPER].asString());
  m_strMediaType = std::move(value[GAME_MEDIA_TYPE].asString());
  m_playerCount = static_cast<unsigned int>(value[GAME_PLAYER_COUNT].asInteger());
  m_bCoop = value[GAME_COOP].asBoolean();
  m_releaseDate.SetFromDBDate(value[GAME_RELEASE_DATE].asString());
  m_addDate.SetFromDBDate(value[GAME_ADD_DATE].asString());
  m_rating = static_cast<unsigned int>(value[GAME_RATING].asInteger());
  m_esbr = static_cast<EsbrRating>(value[GAME_ESBR].asInteger());
  if (value[GAME_RATING].isArray())
  {
    for (auto it = value[GAME_RATING].begin_array(); it != value[GAME_RATING].end_array(); ++it)
    {
      const CVariant& subvalue = *it;
      if (subvalue.isObject())
      {
        GameEdition edition =
          {
            ROM_NO_FLAGS,
            COUNTRY_UNKNOWN,
            "",
            0,
            "",
          };
        edition.flags     = static_cast<GameFlag>(subvalue[GAME_EDITION_FLAGS].asInteger());
        edition.countries = static_cast<CountryCode>(subvalue[GAME_EDITION_COUNTRIES].asInteger());
        edition.path      = std::move(subvalue[GAME_EDITION_PATH].asString());
        edition.crc       = static_cast<uint32_t>(subvalue[GAME_EDITION_CRC].asInteger());
        edition.provider  = std::move(subvalue[GAME_EDITION_PROVIDER].asString());
        m_editions.push_back(edition);
      }
    }
  }
  if (value[GAME_GENRES].isArray())
  {
    for (auto it = value[GAME_GENRES].begin_array(); it != value[GAME_GENRES].end_array(); ++it)
      m_genres.push_back(std::move(it->asString()));
  }
  if (value[GAME_PLATFORMS].isArray())
  {
    for (auto it = value[GAME_PLATFORMS].begin_array(); it != value[GAME_PLATFORMS].end_array(); ++it)
      m_platforms.push_back(std::move(it->asString()));
  }
  if (value[GAME_SERIES].isArray())
  {
    for (auto it = value[GAME_SERIES].begin_array(); it != value[GAME_SERIES].end_array(); ++it)
      m_series.push_back(std::move(it->asString()));
  }
  if (value[GAME_ARTWORK].isArray())
  {
    for (auto it = value[GAME_ARTWORK].begin_array(); it != value[GAME_ARTWORK].end_array(); ++it)
    {
      const CVariant& subvalue = *it;
      if (subvalue.isObject())
      {
        GameArtwork artwork =
          {
            ARTWORK_UNKNOWN,
            "",
          };
        artwork.type = static_cast<GameArtworkType>(subvalue[GAME_ARTWORK_TYPE].asInteger());
        artwork.path = std::move(subvalue[GAME_EDITION_PATH].asString());
        m_artwork.push_back(artwork);
      }
    }
  }
}

void CGameInfoTag::Archive(CArchive& ar)
{
  // TODO
}

void CGameInfoTag::ToSortable(SortItem& sortable, Field field) const
{
  // TODO
}

const char* CGameInfoTag::ESBRString(void) const
{
  return CGameTranslator::GetESBR(m_esbr);
}

void CGameInfoTag::SetESBRString(const std::string& strRating)
{
  m_esbr = CGameTranslator::GetESBR(strRating);
}
