// This file is part of xsonrpc, an XML/JSON RPC library.
// Copyright (C) 2015 Erik Johansson <erik@ejohansson.se>
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef XSONRPC_UTIL_H
#define XSONRPC_UTIL_H

#include <stdint.h>
#include <string>

// namespace tinyxml2 {
// class XMLElement;
// }

struct tm;

namespace xsonrpc {
namespace util {

// bool IsTag(const tinyxml2::XMLElement& element, const char* tag);
// bool HasEmptyText(const tinyxml2::XMLElement& element);

std::string FormatIso8601DateTime(const tm& dt);
bool ParseIso8601DateTime(const char* text, tm& dt);

inline std::string Base64Encode(const std::string& data);
std::string Base64Encode(const char* data, size_t size);

inline std::string Base64Decode(const std::string& str);
std::string Base64Decode(const char* str, size_t size);

} // namespace util
} // namespace xsonrpc

inline std::string xsonrpc::util::Base64Encode(const std::string& data)
{
  return Base64Encode(data.data(), data.size());
}

inline std::string xsonrpc::util::Base64Decode(const std::string& str)
{
  return Base64Decode(str.data(), str.size());
}

#endif
