/*
 *      Copyright (C) 2012-2013 Team XBMC
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

#include "imagefactory.h"
#include "guilib/JpegIO.h"
#include "guilib/cximage.h"
#include "FFmpegImage.h"
#include "utils/Mime.h"

#include <algorithm>

using namespace ADDON;

IImage* ImageFactory::CreateLoader(const std::string& strFileName)
{
  CURL url(strFileName);
  return CreateLoader(url);
}

IImage* ImageFactory::CreateLoader(const CURL& url)
{
  if(!url.GetFileType().empty())
    return CreateLoaderFromMimeType("image/"+url.GetFileType());

  return CreateLoaderFromMimeType(CMime::GetMimeType(url));
}

IImage* ImageFactory::CreateLoaderFromMimeType(const std::string& strMimeType)
{
  VECADDONS codecs;
  CAddonMgr::GetInstance().GetAddons(ADDON_IMAGE_ENCODER, codecs);
  for (auto& codec : codecs)
  {
    std::shared_ptr<CImageEncoder> enc(std::static_pointer_cast<CImageEncoder>(codec));
    std::vector<std::string> mime = StringUtils::Split(enc->GetMimetypes(), "|");
    if (std::find(mime.begin(), mime.end(), strMimeType) != mime.end())
    {
      CImageEncoder* result = new CImageEncoder(*enc);
      result->Create(strMimeType);
      return result;
    }
  }

  if(strMimeType == "image/jpeg" || strMimeType == "image/tbn" || strMimeType == "image/jpg")
    return new CJpegIO();

  return new CXImage(strMimeType);
}

IImage* ImageFactory::CreateFallbackLoader(const std::string& strMimeType)
{
  return new CXImage(strMimeType);
}
