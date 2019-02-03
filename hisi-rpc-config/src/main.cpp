/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ussh
 *
 * Created on 4 декабря 2018 г., 14:09
 */

//#include <cstdlib>
#define LITE
#ifdef LITE

#include <microhttpd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#include "xsonrpc/jsonformathandler.h"
//#include "xsonrpc/server.h"
#include "server.h"
#include "xsonrpc/xmlformathandler.h"
#include "xsonrpc/xmlrpcsystemmethods.h"

//#include "jsonrpc-lean/jsonformathandler.h"
//#include "jsonrpc-lean/jsonwriter.h"
//#include "jsonrpc-lean/request.h"
//#include "jsonrpc-lean/response.h"
//#include "jsonrpc-lean/jsonreader.h"
//#include "jsonrpc-lean/dispatcher.h"
//#include "jsonrpc-lean/fault.h"
//#include "server.h"

#include <iostream>
#include <sstream>
#include <numeric>
#include <poll.h>
#include <string>

//#include "mjp.h"
#include "camera.h"


#define PORT 9999

std::string Ping() {
    return "Pong";
}

/*
 * 
 */
int main(int argc, char** argv) {
    int port = (argv[1] ? atoi(argv[1]) : PORT);
    std::cout << "Port: " << port << std::endl;

    xsonrpc::Server server(port);
    xsonrpc::JsonFormatHandler jsonFormatHandler;
    server.RegisterFormatHandler(jsonFormatHandler);

    auto& dispatcher = server.GetDispatcher();

    bool run = true;
    dispatcher.AddMethod("ping", &Ping);
    dispatcher.AddMethod("AWB.getopt", &AwbGetOpt);
    dispatcher.AddMethod("AWB.setopt", &AwbSetOpt);
    dispatcher.AddMethod("CCM.getopt", &CcmGetOpt);
    dispatcher.AddMethod("CCM.setopt", &CcmSetOpt);
    dispatcher.AddMethod("SAT.getopt", &SaturationGetOpt);
    dispatcher.AddMethod("SAT.setopt", &SaturationSetOpt);
    dispatcher.AddMethod("AE.getopt", &AEGetOpt);
    dispatcher.AddMethod("AE.setopt", &AESetOpt);
    dispatcher.AddMethod("CLTN.getopt", &ColorToneGetOpt);
    dispatcher.AddMethod("CLTN.setopt", &ColorToneSetOpt);
    dispatcher.AddMethod("DRC.getopt", &DRCGetOpt);
    dispatcher.AddMethod("DRC.setopt", &DRCSetOpt);
    dispatcher.AddMethod("GAMMA.getopt", &GammaGetOpt);
    dispatcher.AddMethod("GAMMA.setopt", &GammaSetOpt);
    dispatcher.AddMethod("exit", [&] () {
        run = false; }).SetHidden();

    server.Run();

    pollfd fd;
    fd.fd = server.GetFileDescriptor();
    fd.events = POLLIN;

    while (run && poll(&fd, 1, -1) == 1) {
        server.OnReadableFileDescriptor();
    }
    return 0;
}

#else

#include "camera.h"

int main(int argc, char** argv) {
    fprintf(stderr, "Start");
//    fprintf(stderr, AwbGetOpt().c_str());
//    fprintf(stderr, CcmGetOpt().c_str());
//    fprintf(stderr, SaturationGetOpt().c_str());
//    fprintf(stderr, ColorToneGetOpt().c_str());
//    fprintf(stderr, AEGetOpt().c_str());

    return 0;
}
#endif

