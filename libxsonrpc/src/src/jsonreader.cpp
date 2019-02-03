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

#include "jsonreader.h"

#include "fault.h"
#include "json.h"
#include "request.h"
#include "response.h"
#include "util.h"
#include "value.h"

namespace xsonrpc {

using namespace json;

JsonReader::JsonReader(std::string data)
{
  myDocument.Parse(data.c_str());
  if (myDocument.HasParseError()) {
    throw ParseErrorFault(
      "Parse error: " + std::to_string(myDocument.GetParseError()));
  }
}

Request JsonReader::GetRequest()
{
  if (!myDocument.IsObject()) {
    throw InvalidRequestFault();
  }

  ValidateJsonrpcVersion();

  auto method = myDocument.FindMember(METHOD_NAME);
  if (method == myDocument.MemberEnd() || !method->value.IsString()) {
    throw InvalidRequestFault();
  }

  Request::Parameters parameters;
  auto params = myDocument.FindMember(PARAMS_NAME);
  if (params != myDocument.MemberEnd()) {
    if (!params->value.IsArray()) {
      throw InvalidRequestFault();
    }

    for (auto param = params->value.Begin(); param != params->value.End();
         ++param) {
      parameters.emplace_back(GetValue(*param));
    }
  }

  auto id = myDocument.FindMember(ID_NAME);
  if (id == myDocument.MemberEnd()) {
    // Notification
    return Request(method->value.GetString(), std::move(parameters), false);
  }

  return Request(method->value.GetString(), std::move(parameters),
                 GetId(id->value));
}

Response JsonReader::GetResponse()
{
  if (!myDocument.IsObject()) {
    throw InvalidRequestFault();
  }

  ValidateJsonrpcVersion();

  auto id = myDocument.FindMember(ID_NAME);
  if (id == myDocument.MemberEnd()) {
    throw InvalidRequestFault();
  }

  auto result = myDocument.FindMember(RESULT_NAME);
  auto error = myDocument.FindMember(ERROR_NAME);

  if (result != myDocument.MemberEnd()) {
    if (error != myDocument.MemberEnd()) {
      throw InvalidRequestFault();
    }
    return Response(GetValue(result->value), GetId(id->value));
  }
  else if (error != myDocument.MemberEnd()) {
    if (result != myDocument.MemberEnd()) {
      throw InvalidRequestFault();
    }
    if (!error->value.IsObject()) {
      throw InvalidRequestFault();
    }
    auto code = error->value.FindMember(ERROR_CODE_NAME);
    if (code == error->value.MemberEnd() || !code->value.IsInt()) {
      throw InvalidRequestFault();
    }
    auto message = error->value.FindMember(ERROR_MESSAGE_NAME);
    if (message == error->value.MemberEnd() || !message->value.IsString()) {
      throw InvalidRequestFault();
    }

    return Response(code->value.GetInt(), message->value.GetString(),
                    GetId(id->value));
  }
  else {
    throw InvalidRequestFault();
  }
}

Value JsonReader::GetValue()
{
  return GetValue(myDocument);
}

void JsonReader::ValidateJsonrpcVersion() const
{
  auto jsonrpc = myDocument.FindMember(JSONRPC_NAME);
  if (jsonrpc == myDocument.MemberEnd()
      || !jsonrpc->value.IsString()
      || strcmp(jsonrpc->value.GetString(), JSONRPC_VERSION_2_0) != 0) {
    throw InvalidRequestFault();
  }
}

Value JsonReader::GetValue(const rapidjson::Value& value) const
{
  switch (value.GetType()) {
    case rapidjson::kNullType:
      return Value();
    case rapidjson::kFalseType:
    case rapidjson::kTrueType:
      return Value(value.GetBool());
    case rapidjson::kObjectType: {
      Value::Struct data;
      for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
        std::string name(it->name.GetString(), it->name.GetStringLength());
        data.emplace(name, GetValue(it->value));
      }
      return Value(std::move(data));
    }
    case rapidjson::kArrayType: {
      Value::Array array;
      array.reserve(value.Size());
      for (auto it = value.Begin(); it != value.End(); ++it) {
        array.emplace_back(GetValue(*it));
      }
      return Value(std::move(array));
    }
    case rapidjson::kStringType: {
      tm dt;
      if (util::ParseIso8601DateTime(value.GetString(), dt)) {
        return Value(dt);
      }

      std::string str(value.GetString(), value.GetStringLength());
      const bool binary = str.find('\0') != std::string::npos;
      return Value(std::move(str), binary);
    }
    case rapidjson::kNumberType:
      if (value.IsDouble()) {
        return Value(value.GetDouble());
      }
      else if (value.IsInt()) {
        return Value(value.GetInt());
      }
      else if (value.IsUint()) {
        return Value(static_cast<int64_t>(value.GetUint()));
      }
      else if (value.IsInt64()) {
        return Value(value.GetInt64());
      }
      else {
        assert(value.IsUint64());
        return Value(static_cast<double>(value.GetUint64()));
      }
      break;
  }

  throw InternalErrorFault();
}

Value JsonReader::GetId(const rapidjson::Value& id) const
{
  if (id.IsString()) {
    return id.GetString();
  }
  else if (id.IsInt()) {
    return id.GetInt();
  }
  else if (id.IsInt64()) {
    return id.GetInt64();
  }
  else if (id.IsNull()) {
    return {};
  }

  throw InvalidRequestFault();
}

} // namespace xsonrpc
