//
//  PWTCPSocket.cpp
//  PhotoWeb
//
//  Created by Sanjay Patil on 12/18/15.
//  Copyright © 2015 Self. All rights reserved.
//

#include "PWTCPSocket.hpp"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

PWTCPSocket::PWTCPSocket(int AFType, int type, int proto) {
    FD = socket(AFType, type, proto);
}
struct sockaddr_storage PWTCPSocket::initSocket(char* ipAddress, int port) {
    // Allocate the socket structure for IPv4
    struct sockaddr_storage ss;
    memset(&ss, 0, sizeof(struct sockaddr_storage));
    struct sockaddr_in *sa4 = (struct sockaddr_in*) &ss;
    sa4->sin_family = AF_INET;
    sa4->sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress, &sa4->sin_addr.s_addr) <= 0) {
        printf("Can not assign the requested address due to error:%s",strerror(errno));
    }
    return ss;
}

int PWTCPSocket::setNonblockingOpt(){
    if(fcntl(FD, F_SETFL, O_NONBLOCK)) {
        printf("ERROR:Failed to set the socket to non-blocking mode");
        return -1;
    }
    return 0;
}

int PWTCPSocket::setSocketToReuse() {
    int opt = 1;
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(FD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        printf("Error:Could not set the socket for reuse");
        return -1;
    }
    return 0;
}

int PWTCPSocket::bindSocket(struct sockaddr_storage *sa) {
    if (sa->ss_family == AF_INET) {
        int ret = bind(FD, (struct sockaddr*)sa, sizeof(struct sockaddr_in));
        if (ret < 0) {
            printf("Failed to bind the socket due to error:%s", strerror(errno));
            return ret;
        }
    } else if (sa->ss_family == AF_INET6) {
        int ret = bind(FD, (struct sockaddr*)sa, sizeof(struct sockaddr_in6));
        if (ret < 0) {
            printf("Failed to bind the socket due to error:%s", strerror(errno));
            return ret;
        }
    }
    return 0;
}

int PWTCPSocket::startListen() {
    int ret = listen(FD, 10);
    if (ret == -1) {
        printf("Failed to set the socket to listening mode");
    }
    return ret;
}