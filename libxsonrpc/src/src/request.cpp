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

#include "request.h"
#include "writer.h"

namespace xsonrpc {

Request::Request(std::string methodName, Parameters parameters, Value id)
  : myMethodName(std::move(methodName)),
    myParameters(std::move(parameters)),
    myId(std::move(id))
{
  // Empty
}

void Request::Write(Writer& writer) const
{
  Write(myMethodName, myParameters, myId, writer);
}

void Request::Write(const std::string& methodName, const Parameters& params,
                    const Value& id, Writer& writer)
{
  writer.StartDocument();
  writer.StartRequest(methodName, id);
  for (auto& param : params) {
    writer.StartParameter();
    param.Write(writer);
    writer.EndParameter();
  }
  writer.EndRequest();
  writer.EndDocument();
}

} // namespace xsonrpc
