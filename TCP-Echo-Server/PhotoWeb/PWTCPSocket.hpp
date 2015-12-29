//
//  PWTCPSocket.hpp
//  PhotoWeb
//
//  Created by Sanjay Patil on 12/18/15.
//  Copyright © 2015 Self. All rights reserved.
//

#ifndef PWTCPSocket_hpp
#define PWTCPSocket_hpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

class PWTCPSocket {
protected:
    
public:
    int FD;
    PWTCPSocket(int, int, int );
    
    struct sockaddr_storage initSocket(char* ipAddress, int port);
    int bindSocket(struct sockaddr_storage *sa);
    int setSocketToReuse();
    int setNonblockingOpt();
    int startListen();
};

#endif /* PWTCPSocket_hpp */
