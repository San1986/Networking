//
//  PWTCPServer.hpp
//  PhotoWeb
//
//  Created by Sanjay Patil on 12/23/15.
//  Copyright © 2015 Self. All rights reserved.
//

#ifndef PWTCPServer_hpp
#define PWTCPServer_hpp

#include <stdio.h>
#include "PWTCPSocket.hpp"
class PWTCPServer {
    fd_set master_fds, read_fds;
    int fdmax;
public:
    PWTCPServer(const char *name, int protNumber); // Constructor for TCP Server
    int startPWTCPServer();
protected:
    char hostName[256];
    int port;
    PWTCPSocket *socket;
};
#endif /* PWTCPServer_hpp */
