//
//  dnsResolver.hpp
//  DNSResolver
//
//  Created by Sanjay Patil on 4/19/16.
//

#ifndef dnsResolver_hpp
#define dnsResolver_hpp

#include <stdio.h>
#include "dnsResolverConstants.h"
#include "dnsUDPSocket.hpp"
#include <sys/types.h>
#include <arpa/inet.h>

typedef void (*dns_callback_func)(const unsigned char *ip, int err);
typedef short DNSERR;
static dns_callback_func dns_response_func;

class dnsResolver {
protected:
    
    dnsUDPSocket * dnsForwarder;
    void buildDNSQueryHeader(unsigned char * dns_query, unsigned int type, u_int8_t opcode, int *length);
    const unsigned char * buildDNSQuery(char* host, unsigned int type, u_int8_t opcode, int *length);
public:
    dnsResolver();
    virtual ~dnsResolver();
    DNSERR resolveHostName_WithServer(char*host, const char* dnsserver, dns_callback_func callback, int qtype);
    static void parse_dns_response(const unsigned char *response, int err);
};
#endif /* dnsResolver_hpp */
