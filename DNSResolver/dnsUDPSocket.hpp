//
//  dnsUDPSocket.hpp
//  DNSResolver
//
//  Created by Sanjay Patil on 4/15/16.
//

#ifndef dnsUDPSocket_hpp
#define dnsUDPSocket_hpp

#include <stdio.h>

class dnsResolver;
class dnsUDPSocket {
protected:
    
public:
    int fd;
    virtual ~dnsUDPSocket();
    int bindDNSSocket();
    int sendDNSQuery(const unsigned char* query, size_t len, struct sockaddr_storage* ss);
};


#endif /* dnsUDPSocket_hpp */
