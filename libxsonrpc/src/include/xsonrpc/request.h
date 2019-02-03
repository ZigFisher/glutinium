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

#ifndef XSONRPC_REQUEST_H
#define XSONRPC_REQUEST_H

#include "value.h"

#include <deque>
#include <string>

namespace xsonrpc {

class Writer;

class Request
{
public:
  typedef std::deque<Value> Parameters;

  Request(std::string methodName, Parameters parameters, Value id);

  const std::string& GetMethodName() const { return myMethodName; }
  const Parameters& GetParameters() const { return myParameters; }
  const Value& GetId() const { return myId; }

  void Write(Writer& writer) const;
  static void Write(const std::string& methodName, const Parameters& params,
                    const Value& id, Writer& writer);

private:
  std::string myMethodName;
  Parameters myParameters;
  Value myId;
};

} // namespace xsonrpc

#endif
