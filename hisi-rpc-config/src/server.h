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

#ifndef XSONRPC_SERVER_H
#define XSONRPC_SERVER_H

#include "xsonrpc/dispatcher.h"

#include <string>

#define XSONRPC_VERSION "0.1"

struct MHD_Connection;
struct MHD_Daemon;

namespace xsonrpc {

    class FormatHandler;

    class Server {
    public:
        Server(unsigned short port);
        ~Server();

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        void RegisterFormatHandler(FormatHandler& formatHandler);

        void Run();
        int GetFileDescriptor();
        void OnReadableFileDescriptor();

        Dispatcher& GetDispatcher() {
            return myDispatcher;
        }

    private:
        void HandleRequest(MHD_Connection* connection, void* connectionCls);

        // Callbacks
        static int AccessHandlerCallback(
                void* cls, MHD_Connection* connection,
                const char* url, const char* method, const char* version,
                const char* uploadData, size_t* uploadDataSize,
                void** connectionCls);
        int AccessHandler(
                MHD_Connection* connection,
                const char* url, const char* method, const char* version,
                const char* uploadData, size_t* uploadDataSize,
                void** connectionCls);
        static void RequestCompletedCallback(
                void* cls, MHD_Connection* connection,
                void** connectionCls, int requestTerminationCode);
        void OnRequestCompleted(
                MHD_Connection* connection, void** connectionCls,
                int requestTerminationCode);

        MHD_Daemon* myDaemon;
        Dispatcher myDispatcher;
        std::vector<FormatHandler*> myFormatHandlers;
    };

} // namespace xsonrpc

#endif
