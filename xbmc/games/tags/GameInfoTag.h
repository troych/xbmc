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
#pragma once

#include "dbwrappers/nosql/IDocument.h"
#include "games/GameTypes.h"
#include "utils/IArchivable.h"
#include "utils/ISortable.h"
#include "XBDateTime.h"

#include <string>
#include <vector>

namespace GAME
{
  struct GameEdition
  {
    GameFlag     flags;
    CountryCode  countries;
    std::string  path;
    uint32_t     crc; // CRC, or zero if unknown
    std::string  provider;
  };

  struct GameArtwork
  {
    GameArtworkType type;
    std::string     path;
  };

  class CGameInfoTag : public IArchivable, public dbiplus::IDocument, public ISortable
  {
  public:
    CGameInfoTag() { Reset(); }
    CGameInfoTag(const CGameInfoTag& tag) { *this = tag; }
    const CGameInfoTag& operator=(const CGameInfoTag& tag);
    void Reset();

    CGameInfoTag& Merge(const CGameInfoTag& other);

    virtual void Archive(CArchive& ar) override;
    virtual void Serialize(CVariant& value) const override;
    virtual void Deserialize(const CVariant& value);
    virtual void Deserialize(CVariant&& value) override;
    virtual void ToSortable(SortItem& sortable, Field field) const override;

    /*
     * Single-valued properties
     */
    int64_t            ID(void) const { return m_id; }                   // Unique identifier for this game
    const std::string& Title(void) const { return m_strTitle; }          // Game title
    const std::string& Publisher(void) const { return m_publisher; }     // Game publisher
    const std::string& Developer(void) const { return m_developer; }     // Game developer
    const std::string& MediaType(void) const { return m_strMediaType; }  // Physical media type, e.g. "ROM+MBC5+RAM+BATT" or "CD"
    unsigned int       PlayerCount(void) const { return m_playerCount; } // Number of players supported by the game
    bool               IsCoop(void) const { return m_bCoop; }            // True if game supports cooperative mode
    const CDateTime&   ReleaseDate(void) const { return m_releaseDate; } // Release date to best known accuracy
    const CDateTime&   AddDate(void) const { return m_addDate; }         // Date the game was added to the database
    unsigned int       Rating(void) const { return m_rating; }           // Rating out of a possible 10
    EsbrRating         ESBR(void) const { return m_esbr; }               // ESBR rating
    const char*        ESBRString(void) const;

    void SetID(int64_t identifier) { m_id = identifier; }
    void SetTitle(const std::string& strTitle) { m_strTitle = strTitle; }
    void SetPublisher(const std::string& strPublisher) { m_publisher = strPublisher; }
    void SetDeveloper(const std::string& strDeveloper) { m_developer = strDeveloper; }
    void SetMediaType(const std::string& strMediaType) { m_strMediaType = strMediaType; }
    void SetPlayerCount(unsigned int playerCount) { m_playerCount = playerCount; }
    void SetCoop(bool bIsCoop) { m_bCoop = bIsCoop; }
    void SetReleaseDate(const CDateTime& date) { m_releaseDate = date; }
    void SetAddDate(const CDateTime& date) { m_addDate = date; }
    void SetRating(unsigned int rating) { m_rating = rating; }
    void SetESBR(EsbrRating rating) { m_esbr = rating; }
    void SetESBRString(const std::string& strRating);

    /*
     * Multi-valued properties
     */
    const std::vector<GameEdition>& Editions(void) const  { return m_editions; }   // Alternate editions of the same game
    const std::vector<std::string>& Genres(void) const    { return m_genres; }     // List of genres
    const std::vector<std::string>& Platforms(void) const { return m_platforms; }  // List of platforms this game runs on
    const std::vector<std::string>& Series(void) const    { return m_series; }     // List of series this game belongs to
    const std::vector<GameArtwork>& Artwork(void) const   { return m_artwork; }    // Artwork related to this game

    std::vector<GameEdition>& Editions(void)  { return m_editions; }   // Alternate editions of the same game
    std::vector<std::string>& Genres(void)    { return m_genres; }     // List of genres
    std::vector<std::string>& Platforms(void) { return m_platforms; }  // List of platforms this game runs on
    std::vector<std::string>& Series(void)    { return m_series; }     // List of series this game belongs to
    std::vector<GameArtwork>& Artwork(void)   { return m_artwork; }    // Artwork related to this game

  private:
    int64_t                   m_id;
    std::string               m_strTitle;
    std::string               m_publisher;
    std::string               m_developer;
    std::string               m_strMediaType;
    unsigned int              m_playerCount;
    bool                      m_bCoop;
    CDateTime                 m_releaseDate;
    CDateTime                 m_addDate;
    unsigned int              m_rating;
    EsbrRating                m_esbr;
    std::vector<GameEdition>  m_editions;
    std::vector<std::string>  m_genres;
    std::vector<std::string>  m_platforms;
    std::vector<std::string>  m_series;
    std::vector<GameArtwork>  m_artwork;
  };
}
