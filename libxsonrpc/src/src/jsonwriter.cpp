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

#include "jsonwriter.h"

#include "json.h"
#include "util.h"
#include "value.h"

namespace xsonrpc {

using namespace json;

JsonWriter::JsonWriter()
  : myWriter(myStringBuffer)
{
}

const char* JsonWriter::GetData()
{
  return myStringBuffer.GetString();
}

size_t JsonWriter::GetSize()
{
  return myStringBuffer.GetSize();
}

void JsonWriter::StartDocument()
{
  // Empty
}

void JsonWriter::EndDocument()
{
  // Empty
}

void JsonWriter::StartRequest(const std::string& methodName, const Value& id)
{
  myWriter.StartObject();

  myWriter.Key(JSONRPC_NAME, sizeof(JSONRPC_NAME) - 1);
  myWriter.String(JSONRPC_VERSION_2_0, sizeof(JSONRPC_VERSION_2_0) - 1);

  myWriter.Key(METHOD_NAME, sizeof(METHOD_NAME) - 1);
  myWriter.String(methodName.data(), methodName.size(), true);

  WriteId(id);

  myWriter.Key(PARAMS_NAME, sizeof(PARAMS_NAME) - 1);
  myWriter.StartArray();
}

void JsonWriter::EndRequest()
{
  myWriter.EndArray();
  myWriter.EndObject();
}

void JsonWriter::StartParameter()
{
  // Empty
}

void JsonWriter::EndParameter()
{
  // Empty
}

void JsonWriter::StartResponse(const Value& id)
{
  myWriter.StartObject();

  myWriter.Key(JSONRPC_NAME, sizeof(JSONRPC_NAME) - 1);
  myWriter.String(JSONRPC_VERSION_2_0, sizeof(JSONRPC_VERSION_2_0) - 1);

  WriteId(id);

  myWriter.Key(RESULT_NAME, sizeof(RESULT_NAME) - 1);
}

void JsonWriter::EndResponse()
{
  myWriter.EndObject();
}

void JsonWriter::StartFaultResponse(const Value& id)
{
  myWriter.StartObject();

  myWriter.Key(JSONRPC_NAME, sizeof(JSONRPC_NAME) - 1);
  myWriter.String(JSONRPC_VERSION_2_0, sizeof(JSONRPC_VERSION_2_0) - 1);

  WriteId(id);
}

void JsonWriter::EndFaultResponse()
{
  myWriter.EndObject();
}

void JsonWriter::WriteFault(int32_t code, const std::string& string)
{
  myWriter.Key(ERROR_NAME, sizeof(ERROR_NAME) - 1);
  myWriter.StartObject();

  myWriter.Key(ERROR_CODE_NAME, sizeof(ERROR_CODE_NAME) - 1);
  myWriter.Int(code);

  myWriter.Key(ERROR_MESSAGE_NAME, sizeof(ERROR_MESSAGE_NAME) - 1);
  myWriter.String(string.data(), string.size(), true);

  myWriter.EndObject();
}

void JsonWriter::StartArray()
{
  myWriter.StartArray();
}

void JsonWriter::EndArray()
{
  myWriter.EndArray();
}

void JsonWriter::StartStruct()
{
  myWriter.StartObject();
}

void JsonWriter::EndStruct()
{
  myWriter.EndObject();
}

void JsonWriter::StartStructElement(const std::string& name)
{
  myWriter.Key(name.data(), name.size(), true);
}

void JsonWriter::EndStructElement()
{
  // Empty
}

void JsonWriter::WriteBinary(const char* data, size_t size)
{
  myWriter.String(data, size, true);
}

void JsonWriter::WriteNull()
{
  myWriter.Null();
}

void JsonWriter::Write(bool value)
{
  myWriter.Bool(value);
}

void JsonWriter::Write(double value)
{
  myWriter.Double(value);
}

void JsonWriter::Write(int32_t value)
{
  myWriter.Int(value);
}

void JsonWriter::Write(int64_t value)
{
  myWriter.Int64(value);
}

void JsonWriter::Write(const std::string& value)
{
  myWriter.String(value.data(), value.size(), true);
}

void JsonWriter::Write(const tm& value)
{
  Write(util::FormatIso8601DateTime(value));
}

void JsonWriter::WriteId(const Value& id)
{
  if (id.IsString() || id.IsInteger32() || id.IsInteger64() || id.IsNil()) {
    myWriter.Key(ID_NAME, sizeof(ID_NAME) - 1);
    if (id.IsString()) {
      myWriter.String(id.AsString().data(), id.AsString().size(), true);
    }
    else if (id.IsInteger32()) {
      myWriter.Int(id.AsInteger32());
    }
    else if (id.IsInteger64()) {
      myWriter.Int64(id.AsInteger64());
    }
    else {
      myWriter.Null();
    }
  }
}

} // namespace xsonrpc
