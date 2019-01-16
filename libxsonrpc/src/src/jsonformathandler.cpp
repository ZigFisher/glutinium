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

#include "jsonformathandler.h"

#include "jsonreader.h"
#include "jsonwriter.h"

namespace {

const char APPLICATION_JSON[] = "application/json";

} // namespace

namespace xsonrpc {

JsonFormatHandler::JsonFormatHandler(std::string requestPath)
  : myRequestPath(std::move(requestPath))
{
}

bool JsonFormatHandler::CanHandleRequest(
  const std::string& path, const std::string& contentType)
{
  return path == myRequestPath && contentType == APPLICATION_JSON;
}

std::string JsonFormatHandler::GetContentType()
{
  return APPLICATION_JSON;
}

bool JsonFormatHandler::UsesId()
{
  return true;
}

std::unique_ptr<Reader> JsonFormatHandler::CreateReader(std::string data)
{
  return std::unique_ptr<Reader>(new JsonReader(std::move(data)));
}

std::unique_ptr<Writer> JsonFormatHandler::CreateWriter()
{
  return std::unique_ptr<Writer>(new JsonWriter());
}

} // namespace xsonrpc
