//
//  main.cpp
//  PhotoWeb
//
//  Created by Sanjay Patil on 12/18/15.
//  Copyright © 2015 Self. All rights reserved.
//

#include <iostream>
#include "PWTCPServer.hpp"
int main(int argc, const char * argv[]) {
    // Parse the IP address and port from the arguements
    if (argc < 2) {
        printf("Please provide the IP and Port as command line arguements");
        return 0;
    }
    const char *iP = argv[1];
    int port = atoi(argv[2]);
    printf("Starting the PWTCP Server at [%s:%d]", iP, port);
    // insert code here...
    PWTCPServer *tcpServer = new PWTCPServer(iP, port);
    tcpServer->startPWTCPServer();
    return 0;
}
