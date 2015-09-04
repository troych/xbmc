/*
 *      Copyright (C) 2015 Team Kodi
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "BsonUtils.h"
#include "utils/log.h" // TODO: Until wide strings get fixed
#include "utils/Variant.h"

#include <ejdb/bson.h>

#include <stdio.h>

using namespace dbiplus;

#ifdef WIN32
#define snprintf sprintf_s
#endif

bool CBsonUtils::Write(void* bsonDocument, const char* name, const CVariant &value)
{
  bson* document = static_cast<bson*>(bsonDocument);

  bool success;

  switch (value.type())
  {
  case CVariant::VariantTypeInteger:
    success = bson_append_int(document, name, static_cast<int>(value.asInteger())) == BSON_OK;
    break;
  case CVariant::VariantTypeUnsignedInteger:
    success = bson_append_int(document, name, static_cast<int>(value.asUnsignedInteger())) == BSON_OK;
    break;
  case CVariant::VariantTypeDouble:
    success = bson_append_double(document, name, value.asDouble()) == BSON_OK;
    break;
  case CVariant::VariantTypeBoolean:
    success = bson_append_bool(document, name, value.asBoolean()) == BSON_OK;
    break;
  case CVariant::VariantTypeString:
    success = bson_append_string(document, name, value.asString().c_str()) == BSON_OK;
    break;
  case CVariant::VariantTypeWideString:
    CLog::Log(LOGERROR, "Error writing property \"%s\": wide string not implemented in BSON writer", name);
    success = true; // TODO
    break;
  case CVariant::VariantTypeArray:
    {
      success = bson_append_start_array(document, name) == BSON_OK;

      // For the array, we have to manually set the index values from "0" to "N"
      unsigned int i = 0;
      char index[12];
      for (CVariant::const_iterator_array it = value.begin_array(); it != value.end_array() && success; it++)
      {
        snprintf(index, sizeof(index), "%u", i++);
        success &= Write(document, index, *it);
      }
      success &= bson_append_finish_array(document) == BSON_OK;
    }
    break;
  case CVariant::VariantTypeObject:
    {
      success = bson_append_start_object(document, name) == BSON_OK;

      for (CVariant::const_iterator_map it = value.begin_map(); it != value.end_map() && success; it++)
        success &= Write(document, it->first.c_str(), it->second);

      success &= bson_append_finish_object(document) == BSON_OK;

      break;
    }
  case CVariant::VariantTypeNull:
  case CVariant::VariantTypeConstNull:
  default:
    success = bson_append_null(document, name) == BSON_OK;
    break;
  }

  return success;
}

CVariant CBsonUtils::Read(int type, void* bsonIterator)
{
  CVariant output;

  bson_iterator* it = static_cast<bson_iterator*>(bsonIterator);

  switch (type)
  {
  case BSON_INT:
    output = bson_iterator_int(it);
    break;
  case BSON_LONG:
    output = bson_iterator_long(it);
    break;
  case BSON_DOUBLE:
    output = bson_iterator_double(it);
    break;
  case BSON_BOOL:
    output = bson_iterator_bool(it);
    break;
  case BSON_STRING:
    output = bson_iterator_string(it);
    break;
  case BSON_ARRAY:
  case BSON_OBJECT:
    {
      bson_iterator subit[1];
      bson_iterator_subiterator(it, subit);

      bson_type subtype;
      subtype = bson_iterator_next(subit);

      while (subtype != BSON_EOO)
      {
        if (type == BSON_ARRAY)
          output.push_back(Read(subtype, subit));
        else
          output[bson_iterator_key(subit)] = Read(subtype, subit);
        subtype = bson_iterator_next(subit);
      }
    }
  case BSON_NULL:
  default:
    break;
  }

  return output;
}
