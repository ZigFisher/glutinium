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

#ifndef XSONRPC_JSONWRITER_H
#define XSONRPC_JSONWRITER_H

#include "writer.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace xsonrpc {

class JsonWriter final : public Writer
{
public:
  JsonWriter();

  // Writer
  const char* GetData() override;
  size_t GetSize() override;
  void StartDocument() override;
  void EndDocument() override;
  void StartRequest(const std::string& methodName, const Value& id) override;
  void EndRequest() override;
  void StartParameter() override;
  void EndParameter() override;
  void StartResponse(const Value& id) override;
  void EndResponse() override;
  void StartFaultResponse(const Value& id) override;
  void EndFaultResponse() override;
  void WriteFault(int32_t code, const std::string& string) override;
  void StartArray() override;
  void EndArray() override;
  void StartStruct() override;
  void EndStruct() override;
  void StartStructElement(const std::string& name) override;
  void EndStructElement() override;
  void WriteBinary(const char* data, size_t size) override;
  void WriteNull() override;
  void Write(bool value) override;
  void Write(double value) override;
  void Write(int32_t value) override;
  void Write(int64_t value) override;
  void Write(const std::string& value) override;
  void Write(const tm& value) override;

private:
  void WriteId(const Value& id);

  rapidjson::StringBuffer myStringBuffer;
  rapidjson::Writer<rapidjson::StringBuffer> myWriter;
};

} // namespace xsonrpc

#endif
