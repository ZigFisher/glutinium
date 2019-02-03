[![Build Status](https://travis-ci.org/erijo/xsonrpc.svg?branch=master)](https://travis-ci.org/erijo/xsonrpc)

# xsonrpc

An [LGPL](http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) licensed
XML/JSON RPC library for C++11.


## Examples

A simple server handling both [XML-RPC](http://xmlrpc.scripting.com/spec.html)
and [JSON-RPC:](http://www.jsonrpc.org/specification)

```C++
std::string Concat(const std::string& a, const std::string& b)
{
  return a + b;
}

int main()
{
  xsonrpc::Server server(8080);

  xsonrpc::JsonFormatHandler jsonFormatHandler;
  xsonrpc::XmlFormatHandler xmlFormatHandler;
  server.RegisterFormatHandler(jsonFormatHandler);
  server.RegisterFormatHandler(xmlFormatHandler);

  auto& dispatcher = server.GetDispatcher();
  dispatcher.AddMethod("concat", &Concat);

  bool run = true;
  dispatcher.AddMethod("exit", [&] () { run = false; }).SetHidden();

  server.Run();

  pollfd fd;
  fd.fd = server.GetFileDescriptor();
  fd.events = POLLIN;

  while (run && poll(&fd, 1, -1) == 1) {
    server.OnReadableFileDescriptor();
  }

  return 0;
}
```

A client capable of calling the server above could look like this:

```C++
int main(int argc, char** argv)
{
  xsonrpc::Client::GlobalInit();

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
  xsonrpc::Client client("localhost", 8080, *formatHandler);

  std::cout << client.Call("concat", "Hello, ", "World!") << "\n";
  client.Call("exit");
  return 0;
}
```

## Build instructions

To build xsonrpc you need:

* A C++11 capable compiler (e.g. GCC 4.8 or newer)
* [cmake](http://www.cmake.org/)
* [libcurl](http://curl.haxx.se/libcurl/)
* [libmicrohttpd](http://www.gnu.org/software/libmicrohttpd/)

```Shell
mkdir build
cd build
cmake path/to/xsonrpc
make
```
