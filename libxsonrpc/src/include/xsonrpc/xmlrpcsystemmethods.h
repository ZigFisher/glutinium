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

#ifndef XSONRPC_XMLRPCSYSTEMMETHODS_H
#define XSONRPC_XMLRPCSYSTEMMETHODS_H

#include "request.h"
#include "value.h"

#include <map>

namespace xsonrpc {

class Dispatcher;

class XmlRpcSystemMethods
{
public:
  XmlRpcSystemMethods(Dispatcher& dispatcher, bool introspection);
  ~XmlRpcSystemMethods();

  void AddCapability(std::string name, std::string url, int32_t version);
  void RemoveCapability(const std::string& name);

private:
  Value SystemMulticall(const Request::Parameters& parameters) const;
  Value SystemListMethods() const;
  Value SystemMethodSignature(const std::string& methodName) const;
  std::string SystemMethodHelp(const std::string& methodName) const;
  Value SystemGetCapabilities() const;

  Dispatcher& myDispatcher;

  struct Capability
  {
    std::string Url;
    int32_t Version;
  };
  std::map<std::string, Capability> myCapabilities;
};

} // namespace xsonrpc

#endif
