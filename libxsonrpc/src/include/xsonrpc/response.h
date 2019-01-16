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

#ifndef XSONRPC_RESPONSE_H
#define XSONRPC_RESPONSE_H

#include "value.h"

namespace xsonrpc {

class Writer;

class Response
{
public:
  Response(Value value, Value id);
  Response(int32_t faultCode, std::string faultString, Value id);

  void Write(Writer& writer) const;

  Value& GetResult() { return myResult; }
  bool IsFault() const { return myIsFault; }
  void ThrowIfFault() const;
  const Value& GetId() const { return myId; }

private:
  Value myResult;
  bool myIsFault;
  int32_t myFaultCode;
  std::string myFaultString;
  Value myId;
};

} // namespace xsonrpc

#endif
