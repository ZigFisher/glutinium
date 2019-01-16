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

#include "xsonrpc/client.h"
#include "xsonrpc/fault.h"
#include "xsonrpc/jsonformathandler.h"
#include "xsonrpc/xmlformathandler.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <memory>

void LogArguments() { }

template<typename Head>
void LogArguments(Head&& head)
{
  std::cout << head;
}

template<typename Head, typename... Tail>
void LogArguments(Head&& head, Tail&&... tail)
{
  std::cout << head << ", ";
  LogArguments(std::forward<Tail>(tail)...);
}

void LogArguments(xsonrpc::Request::Parameters& params)
{
  for (auto it = params.begin(); it != params.end(); ++it) {
    if (it != params.begin()) {
      std::cout << ", ";
    }
    std::cout << *it;
  }
}

size_t CallErrors = 0;

template<typename... T>
void LogCall(xsonrpc::Client& client, std::string method, T&&... args)
{
  std::cout << method << '(';
  LogArguments(std::forward<T>(args)...);
  std::cout << "):\n>>> ";
  try {
    std::cout << client.Call(std::move(method), std::forward<T>(args)...);
  }
  catch (const xsonrpc::Fault& fault) {
    ++CallErrors;
    std::cout << "Error: " << fault.what();
  }
  std::cout << "\n\n";
}

int main(int argc, char** argv)
{
  // Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " HOST" << std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }
  int port = (argv[2]? atoi(argv[2]) : 8080 );
  // int host = argv[1];
  const std::string host = (argv[1]? argv[1] : "localhost" );
  // int host = (argv[1]? atoi(argv[1]) : "localhost" );
  std::unique_ptr<xsonrpc::FormatHandler> formatHandler;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "json") == 0) {
      std::cout << "Using JSON format\n";
      formatHandler.reset(new xsonrpc::JsonFormatHandler());
      break;
    }
  }
  if (!formatHandler) {
    std::cout << "Using XML format\n";
    formatHandler.reset(new xsonrpc::XmlFormatHandler());
  }

  try {
    xsonrpc::Client::GlobalInit();
    xsonrpc::Client client(host, port, *formatHandler);

    LogCall(client, "add", 3, 2);
    LogCall(client, "concat", "Hello, ", "World!");

    xsonrpc::Request::Parameters params;
    {
      xsonrpc::Value::Array a;
      a.emplace_back(1000);
      a.emplace_back(std::numeric_limits<int32_t>::max());
      params.push_back(std::move(a));
    }
    LogCall(client, "add_array", params);

    LogCall(client, "to_binary", "Hello World!");
    LogCall(client, "from_binary", xsonrpc::Value("Hi!", true));

    params.clear();
    {
      xsonrpc::Value::Array a;
      a.emplace_back(12);
      a.emplace_back("foobar");
      a.emplace_back(a);
      params.push_back(std::move(a));
    }
    LogCall(client, "to_struct", params);

    params.clear();
    {
      xsonrpc::Value::Array calls;
      {
        xsonrpc::Value::Struct call;
        call["methodName"] = "add";
        {
          xsonrpc::Value::Array params;
          params.emplace_back(23);
          params.emplace_back(19);
          call["params"] = std::move(params);
        }
        calls.emplace_back(std::move(call));
      }
      {
        xsonrpc::Value::Struct call;
        call["methodName"] = "does.NotExist";
        calls.emplace_back(std::move(call));
      }
      {
        xsonrpc::Value::Struct call;
        call["methodName"] = "concat";
        {
          xsonrpc::Value::Array params;
          params.emplace_back("Hello ");
          params.emplace_back("multicall!");
          call["params"] = std::move(params);
        }
        calls.emplace_back(std::move(call));
      }
      params.emplace_back(std::move(calls));
    }
    LogCall(client, "system.multicall", params);
    LogCall(client, "system.listMethods");
    LogCall(client, "system.methodHelp", "add");
    LogCall(client, "system.methodSignature", "add");

    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-e") == 0) {
        LogCall(client, "exit");
        break;
      }
    }
  }
  catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }

  if (CallErrors > 0) {
    std::cerr << "Error: " << CallErrors << " call(s) failed\n";
    return 1;
  }

  return 0;
}
