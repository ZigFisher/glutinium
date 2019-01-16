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

#ifndef XSONRPC_JSONFORMATHANDLER_H
#define XSONRPC_JSONFORMATHANDLER_H

#include "formathandler.h"

namespace xsonrpc {

class JsonFormatHandler : public FormatHandler
{
public:
  explicit JsonFormatHandler(std::string requestPath = "/RPC2");

  // FormatHandler
  bool CanHandleRequest(const std::string& path,
                        const std::string& contentType) override;
  std::string GetContentType() override;
  bool UsesId() override;
  std::unique_ptr<Reader> CreateReader(std::string data) override;
  std::unique_ptr<Writer> CreateWriter() override;

private:
  std::string myRequestPath;
};

} // namespace xsonrpc

#endif
