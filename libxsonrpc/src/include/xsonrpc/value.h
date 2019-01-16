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

#ifndef XSONRPC_VALUE_H
#define XSONRPC_VALUE_H

#include <cstdint>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct tm;

namespace xsonrpc {

class Writer;

class Value
{
public:
  typedef std::vector<Value> Array;
  typedef tm DateTime;
  typedef std::string String;
  typedef std::map<std::string, Value> Struct;

  enum class Type
  {
    ARRAY,
    BINARY,
    BOOLEAN,
    DATE_TIME,
    DOUBLE,
    INTEGER_32,
    INTEGER_64,
    NIL,
    STRING,
    STRUCT
  };

  Value() : myType(Type::NIL) {}
  Value(Array value);
  Value(bool value) : myType(Type::BOOLEAN) { as.myBoolean = value; }
  Value(const DateTime& value);
  Value(double value) : myType(Type::DOUBLE) { as.myDouble = value; }
  Value(int32_t value);
  Value(int64_t value);
  Value(const char* value) : Value(String(value)) {}
  Value(String value, bool binary = false);
  Value(Struct value);

  template<typename T>
  Value(std::vector<T> value)
    : Value(Array{})
  {
    as.myArray->reserve(value.size());
    for (auto& v : value) {
      as.myArray->emplace_back(std::move(v));
    }
  }

  template<typename T>
  Value(const std::map<std::string, T>& value)
    : Value(Struct{})
  {
    for (auto& v : value) {
      as.myStruct->emplace(v.first, v.second);
    }
  }

  template<typename T>
  Value(const std::unordered_map<std::string, T>& value)
    : Value(Struct{})
  {
    for (auto& v : value) {
      as.myStruct->emplace(v.first, v.second);
    }
  }

  ~Value();

  explicit Value(const Value&);
  Value& operator=(const Value&) = delete;
  Value(Value&& other) noexcept;
  Value& operator=(Value&& other) noexcept;

  bool IsArray() const { return myType == Type::ARRAY; }
  bool IsBinary() const { return myType == Type::BINARY; }
  bool IsBoolean() const { return myType == Type::BOOLEAN; }
  bool IsDateTime() const { return myType == Type::DATE_TIME; }
  bool IsDouble() const { return myType == Type::DOUBLE; }
  bool IsInteger32() const { return myType == Type::INTEGER_32; }
  bool IsInteger64() const { return myType == Type::INTEGER_64; }
  bool IsNil() const { return myType == Type::NIL; }
  bool IsString() const { return myType == Type::STRING; }
  bool IsStruct() const { return myType == Type::STRUCT; }

  const Array& AsArray() const;
  const String& AsBinary() const { return AsString(); }
  const bool& AsBoolean() const;
  const DateTime& AsDateTime() const;
  const double& AsDouble() const;
  const int32_t& AsInteger32() const;
  const int64_t& AsInteger64() const;
  const String& AsString() const;
  const Struct& AsStruct() const;

  template<typename T>
  inline const T& AsType() const;

  Type GetType() const { return myType; }

  void Write(Writer& writer) const;

  inline const Value& operator[](Array::size_type i) const;
  inline const Value& operator[](const Struct::key_type& key) const;

private:
  void Reset();

  Type myType;
  union
  {
    Array* myArray;
    bool myBoolean;
    DateTime* myDateTime;
    String* myString;
    Struct* myStruct;
    struct
    {
      double myDouble;
      int32_t myInteger32;
      int64_t myInteger64;
    };
  } as;
};

template<> inline
const Value::Array& Value::AsType<typename Value::Array>() const
{
  return AsArray();
}

template<> inline
const bool& Value::AsType<bool>() const
{
  return AsBoolean();
}

template<> inline
const Value::DateTime& Value::AsType<typename Value::DateTime>() const
{
  return AsDateTime();
}

template<> inline
const double& Value::AsType<double>() const
{
  return AsDouble();
}

template<> inline
const int32_t& Value::AsType<int32_t>() const
{
  return AsInteger32();
}

template<> inline
const int64_t& Value::AsType<int64_t>() const
{
  return AsInteger64();
}

template<> inline
const Value::String& Value::AsType<typename Value::String>() const
{
  return AsString();
}

template<> inline
const Value::Struct& Value::AsType<typename Value::Struct>() const
{
  return AsStruct();
}

template<> inline
const Value& Value::AsType<Value>() const
{
  return *this;
}

inline const Value& Value::operator[](Array::size_type i) const
{
  return AsArray().at(i);
};

inline const Value& Value::operator[](const Struct::key_type& key) const
{
  return AsStruct().at(key);
}

std::ostream& operator<<(std::ostream& os, const Value& value);

} // namespace xsonrpc

#endif
