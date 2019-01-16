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

#include "../src/util.h"

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " -e|-d|-v <FILE>\n";
    return 1;
  }

  if (argv[1][0] != '-' || argv[1][1] == '\0' || argv[1][2] != '\0') {
    return 1;
  }

  const char flag = argv[1][1];
  const bool verify = flag == 'v';
  const bool encode = verify || flag == 'e';
  const bool decode = flag == 'd';
  if (!encode && !decode) {
    return 1;
  }

  int fd = open(argv[2], O_RDONLY);
  if (fd == -1) {
    return 1;
  }

  const size_t size = encode ? 28671 : 30030;
  std::unique_ptr<char[]> buffer(new char[size]);

  ssize_t res;
  while ((res = read(fd, buffer.get(), size)) > 0) {
    if (encode) {
      auto str = xsonrpc::util::Base64Encode(buffer.get(), res);
      if (!verify) {
        std::cout << str << "\r\n";
      }
      else {
        auto binary = xsonrpc::util::Base64Decode(str);
        assert(binary.size() == static_cast<size_t>(res));
        assert(memcmp(buffer.get(), binary.data(), res) == 0);
      }
    }
    else {
      auto binary = xsonrpc::util::Base64Decode(
        reinterpret_cast<const char*>(buffer.get()), res);
      std::cout.write(
        reinterpret_cast<const char*>(binary.data()), binary.size());
    }
  }

  close(fd);
  return 0;
}
