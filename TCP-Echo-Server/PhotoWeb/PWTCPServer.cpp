//
//  PWTCPServer.cpp
//  PhotoWeb
//
//  Created by Sanjay Patil on 12/23/15.
//  Copyright © 2015 Self. All rights reserved.
//

#include "PWTCPServer.hpp"
#include <memory.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/select.h>
PWTCPServer::PWTCPServer(const char *name, int portNumber) {
    memcpy(hostName, name, strlen(name));
    port = portNumber;
    socket = new PWTCPSocket(AF_INET, SOCK_STREAM, 0);
}

static void
getIPFromSockStructure(struct sockaddr_storage *ss, char *addr) {
    if (ss->ss_family == AF_INET) {
        inet_ntop(AF_INET, &((struct sockaddr_in*) ss)->sin_addr.s_addr, addr, sizeof(addr));
    } else {
        inet_ntop(AF_INET6, &((struct sockaddr_in6*) ss)->sin6_addr.s6_addr, addr, sizeof(addr));
    }
}

int PWTCPServer::startPWTCPServer() {
    struct sockaddr_storage ss = socket->initSocket(hostName, port);
    if(socket->setNonblockingOpt() != 0) {
        return -1;
    }
    if(socket->setSocketToReuse() != 0) {
        return -1;
    }
    if (socket->bindSocket(&ss) != 0) {
        return -1;
    }
    if(socket->startListen() != 0) {
        return -1;
    }
    // add the FD for listening socket to FDSet:
    FD_SET(socket->FD, &master_fds);
    fdmax = socket->FD;
    
    //Now wait for the events on the listening socket
    for (;;) {
        read_fds = master_fds; //TODO: Why to do this
        // Add the socket to watch list using "select" API
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) { // this call blocks until some event occurs on the socket
            printf("Could not add the sockts to watch list");
            return -1;
        }
        // we are here as we have some socket event to process
        // look-up the list of socke to test for the event
        for (int i =0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                char addr[256];
                getIPFromSockStructure(&ss, addr);
                // see if we have new connection event on the listening socket
                if (i == socket->FD) {
                    // we have new connection now so accept it
                    struct sockaddr_storage ss;
                    socklen_t len = sizeof(struct sockaddr_storage);
                    int newFD = accept(socket->FD, (struct sockaddr*) &ss, &len);
                    if(newFD == -1){
                        printf("Failed to accept the client's connection request");
                        return -1;
                    }
                    FD_SET(newFD, &master_fds); // add to master set
                    if (newFD > fdmax) {    // keep track of the max
                        fdmax = newFD;
                    }
                    printf("We have a new connection from address:%s",addr);
                } else {
                    // we have new data to be read from the peer connection
                    // read the data from the socket now
                    char buffer[1024];
                    memset(buffer, 0, 1024);
                    int nBytes = 0;
                    if((nBytes = (int)recv(i, buffer, 1024, 0)) <= 0) {
                        if (nBytes <= 0) {
                            // connection closed by peer
                            printf("connection is closed by peer:%s",addr);
                        } else {
                            printf("Could not read from peer:%s",addr);
                        }
                    } else {
                        // read the data and echo back to the peer
                        size_t ret = send(i, buffer, 1024, 0);
                        if (ret == -1) {
                            printf("Could not send the data to peer:%s",addr);
                        }
                    }
                }
            }
            
        }
    }
    return 0;
}