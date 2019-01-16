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

#ifndef XSONRPC_CLIENT_H
#define XSONRPC_CLIENT_H

#include "request.h"
#include "value.h"

#include <functional>
#include <string>

namespace xsonrpc {

class FormatHandler;

class Client
{
public:
  static void GlobalInit();

  Client(const std::string& host, unsigned short port,
         FormatHandler& formatHandler,
         const std::string& uri = "/RPC2");
  ~Client();

  Value Call(const std::string& methodName,
             const Request::Parameters& params = {})
  {
    return CallInternal(methodName, params);
  }

  template<typename FirstType,
           typename... RestTypes>
  typename std::enable_if<
    !std::is_same<typename std::decay<FirstType>::type,
                  Request::Parameters>::value,
    Value>::type
  Call(const std::string& methodName, FirstType&& first,
       RestTypes&&... rest)
  {
    Request::Parameters params;
    params.emplace_back(std::forward<FirstType>(first));

    return CallInternal(
      methodName, params, std::forward<RestTypes>(rest)...);
  }

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
  Client(Client&&) = delete;
  Client& operator=(Client&&) = delete;

private:
  template<typename FirstType, typename... RestTypes>
  Value CallInternal(const std::string& methodName,
                     Request::Parameters& params,
                     FirstType&& first, RestTypes&&... rest)
  {
    params.emplace_back(std::forward<FirstType>(first));
    return CallInternal(
      methodName, params, std::forward<RestTypes>(rest)...);
  }
  Value CallInternal(const std::string& methodName,
                     const Request::Parameters& params);
  FormatHandler& myFormatHandler;
  void* myHandle;
  int32_t myId;
};

} // namespace xsonrpc

#endif
