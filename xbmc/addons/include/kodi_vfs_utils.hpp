/*
 *      Copyright (C) 2015 Team XBMC
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
#pragma once

/*!
 * @file kodi_vfs_utils.hpp - C++ wrappers for Kodi's VFS operations
 */

#include "libXBMC_addon.h"
#include "kodi_vfs_types.h"

#include <stdint.h>
#include <string>
#include <vector>

/* Platform dependent path separator */
#ifndef PATH_SEPARATOR_CHAR
  #if (defined(_WIN32) || defined(_WIN64))
    #define PATH_SEPARATOR_CHAR '\\'
  #else
    #define PATH_SEPARATOR_CHAR '/'
  #endif
#endif

namespace ADDON
{
  class CVFSFile
  {
  public:
    /*!
     * @brief Construct a new, unopened file
     * @param XBMC The libXBMC helper instance. Must remain valid during the lifetime of this file.
     */
    CVFSFile(ADDON::CHelper_libXBMC_addon* frontend) : m_frontend(frontend), m_pFile(nullptr) { }

    /*!
     * @brief Close() is called from the destructor, so explicitly closing the file isn't required
     */
    virtual ~CVFSFile() { Close(); }

    /*!
     * @brief Open a file via XBMC's CFile
     * @param strFileName The filename to open
     * @param flags The flags to pass. Documented in XBMC's File.h
     * @return True on success or false on failure
     */
    bool Open(const std::string &strFileName, unsigned int flags = 0)
    {
      if (!m_frontend)
        return false;
      Close();
      m_pFile = m_frontend->OpenFile(strFileName.c_str(), flags);
      return m_pFile != nullptr;
    }

    /*!
     * @brief Open a file via XBMC's CFile in write mode. The file (and all
     *        subdirectories) will be created if necessary.
     * @param strFileName The filename to open
     * @param bOverWrite True to overwrite, false otherwise
     * @return True on success or false on failure
     */
    bool OpenForWrite(const std::string& strFileName, bool bOverWrite = false)
    {
      if (!m_frontend)
        return false;
      Close();

      // Try to open the file. If it fails, check if we need to create the directory first
      // This way we avoid checking if the directory exists every time
      m_pFile = m_frontend->OpenFileForWrite(strFileName.c_str(), bOverWrite);
      if (!m_pFile)
      {
        std::string cacheDirectory = GetDirectoryName(strFileName);
        if (m_frontend->DirectoryExists(cacheDirectory.c_str()) || m_frontend->CreateDirectory(cacheDirectory.c_str()))
          m_pFile = m_frontend->OpenFileForWrite(strFileName.c_str(), bOverWrite);
      }
      return m_pFile != nullptr;
    }

    /*!
     * @brief Read from an open file
     * @param lpBuffer The buffer to store the data in
     * @param uiBufferSize The size of the buffer
     * @return Number of bytes read
     */
    int64_t Read(void *lpBuffer, uint64_t uiBufferSize)
    {
      if (!m_pFile)
        return 0;
      return m_frontend->ReadFile(m_pFile, lpBuffer, static_cast<size_t>(uiBufferSize));
    }

    /*!
     * @brief Read a string from an open file
     * @param strLine The buffer to store the data in
     * @return True when a line was read, false otherwise
     */
    bool ReadLine(std::string &strLine)
    {
      strLine.clear();
      if (!m_pFile)
        return false;
      // TODO: Read 1024 chars into buffer. If file position advanced that many
      // chars, we didn't hit a newline. Otherwise, if file position is 1 or 2
      // past the number of bytes read, we read (and skipped) a newline sequence.
      char buffer[1025];
      if (m_frontend->ReadFileString(m_pFile, buffer, sizeof(buffer))) // will read 1024 bytes
      {
        strLine = buffer;
        return !strLine.empty();
      }
      return false;
    }

    /*!
     * @brief Write to a file opened in write mode
     * @param lpBuffer The data to write
     * @param uiBufferSize Size of the data to write
     * @return The number of bytes written
     */
    int64_t Write(const void* lpBuffer, uint64_t uiBufferSize)
    {
      if (!m_pFile)
        return 0;
      return m_frontend->WriteFile(m_pFile, lpBuffer, static_cast<size_t>(uiBufferSize));
    }

    /*!
     * @brief Flush buffered data
     */
    void Flush()
    {
      if (!m_pFile)
        return;
      m_frontend->FlushFile(m_pFile);
    }

    /*!
     * @brief Seek in an open file
     * @param iFilePosition The new position
     * @param iWhence Seek argument, see stdio.h for possible values
     * @return The new position
     */
    int64_t Seek(int64_t iFilePosition, int iWhence = SEEK_SET)
    {
      if (!m_pFile)
        return 0;
      return m_frontend->SeekFile(m_pFile, iFilePosition, iWhence);
    }

    /*!
     * @brief Truncate a file to the requested size
     * @param iSize The new max size
     * @return The new size
     */
    int64_t Truncate(uint64_t iSize)
    {
      if (!m_pFile)
        return -1;
      return m_frontend->TruncateFile(m_pFile, iSize);
    }

    /*!
     * @brief The current position in an open file
     * @return The current position
     */
    int64_t GetPosition() const
    {
      if (!m_pFile)
        return -1;
      return m_frontend->GetFilePosition(m_pFile);
    }

    /*!
     * @brief Get the file size of an open file
     * @return The total file size
     */
    int64_t GetLength()
    {
      if (!m_pFile)
        return 0;
      return m_frontend->GetFileLength(m_pFile);
    }

    /*!
     * @brief Close an open file
     */
    void Close()
    {
      if (!m_pFile)
        return;
      m_frontend->CloseFile(m_pFile);
      m_pFile = nullptr;
    }

    /*!
     * @brief Get the minimum size that can be read from the open file. For example,
     *        CDROM files in which access could be sector-based. It can also be
     *        used to indicate a file system is non-buffered but accepts any read
     *        size, in which case GetChunkSize() should return the value 1.
     * @return The chunk size
     */
    int GetChunkSize()
    {
      if (!m_pFile)
        return 0;
      return m_frontend->GetFileChunkSize(m_pFile);
    }

    /*!
     * @brief Return a size aligned to the chunk size at least as large as the chunk size.
     * @param chunk The chunk size
     * @param minimum The minimum size (or maybe the minimum number of chunks?)
     * @return The aligned size
     */
    static unsigned int GetChunkSize(unsigned int chunk, unsigned int minimum)
    {
      if (chunk)
        return chunk * ((minimum + chunk - 1) / chunk);
      else
        return minimum;
    }

    static std::string GetFileName(const std::string &path, char separator = PATH_SEPARATOR_CHAR)
    {
      size_t pos = path.find_last_of(separator);
      return path.substr(pos + 1);
    }

    static std::string GetDirectoryName(const std::string &path, char separator = PATH_SEPARATOR_CHAR)
    {
      size_t pos = path.find_last_of(separator);
      return path.substr(0, pos);
    }

  private:
    ADDON::CHelper_libXBMC_addon* const m_frontend;
    void* m_pFile;
  };

  class CVFSProperty
  {
  public:
    CVFSProperty(const char* name, const char* value) :
      m_name(name ? name : ""),
      m_value(value ? value : "")
    {
    }

    const std::string& Name(void) const { return m_name; }
    const std::string& Value(void) const { return m_value; }

    void SetName(const std::string& name) { m_name = name; }
    void SetValue(const std::string& value) { m_value = value; }

  private:
    std::string m_name;
    std::string m_value;
  };

  class CVFSDirEntry
  {
  public:
    CVFSDirEntry(const std::string& label = "",
                 const std::string& path = "",
                 bool bFolder = false,
                 int64_t size = -1) :
      m_label(label),
      m_path(path),
      m_bFolder(bFolder),
      m_size(size)
    {
    }

    CVFSDirEntry(const VFSDirEntry& dirEntry) :
      m_label(dirEntry.label ? dirEntry.label : ""),
      m_path(dirEntry.path ? dirEntry.path : ""),
      m_bFolder(dirEntry.folder),
      m_size(dirEntry.size)
    {
      for (unsigned int i = 0; i < dirEntry.num_props; i++)
        m_properties.push_back(CVFSProperty(dirEntry.properties[i].name, dirEntry.properties[i].val));
    }

    const std::string& Label(void) const { return m_label; }
    const std::string& Path(void) const { return m_path; }
    bool IsFolder(void) const { return m_bFolder; }
    int64_t Size(void) const { return m_size; }
    const std::vector<CVFSProperty>& Properties(void) const { return m_properties; }

    void SetLabel(const std::string& label) { m_label = label; }
    void SetPath(const std::string& path) { m_path = path; }
    void SetFolder(bool bFolder) { m_bFolder = bFolder; }
    void SetSize(int64_t size) { m_size = size; }
    std::vector<CVFSProperty>& Properties(void) { return m_properties; }

  private:
    std::string m_label;
    std::string m_path;
    bool m_bFolder;
    int64_t m_size;
    std::vector<CVFSProperty> m_properties;
  };

  class VFSUtils
  {
  public:
    static bool GetDirectory(ADDON::CHelper_libXBMC_addon* frontend,
                             const std::string& path,
                             const std::string& mask,
                             std::vector<CVFSDirEntry>& items)
    {
      VFSDirEntry* dir_list = nullptr;
      unsigned int num_items = 0;
      if (frontend->GetDirectory(path.c_str(), mask.c_str(), &dir_list, &num_items))
      {
        for (unsigned int i = 0; i < num_items; i++)
          items.push_back(CVFSDirEntry(dir_list[i]));

        frontend->FreeDirectory(dir_list, num_items);

        return true;
      }
      return false;
    }
  };
}
