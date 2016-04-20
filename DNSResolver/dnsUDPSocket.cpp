//
//  dnsUDPSocket.cpp
//  DNSResolver
//
//  Created by Sanjay Patil on 4/15/16.
//
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "dnsUDPSocket.hpp"
#include "dnsResolverConstants.h"
#include "dnsResolver.hpp"
int socket_fd = 0;

dnsUDPSocket::~dnsUDPSocket()
{
    close(fd);
}
void socket_event_handler(int event)
{
    // Call recvfrom here to read the data from the DNS Server
    struct sockaddr_storage ss;
    unsigned char udp_response[512];
    memset(udp_response, 0, sizeof(udp_response));
    socklen_t len = sizeof(ss);
    long ret = recvfrom(socket_fd, udp_response, 512, MSG_TRUNC, (struct sockaddr*)&ss, &len);
    if (ret > 0) {
        dnsResolver::parse_dns_response(udp_response, ret);
    }
}
int dnsUDPSocket::bindDNSSocket()
{
    struct sigaction handler;
    if((fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        DNSlog_err("Socket Creation Failed");
        return DNSErr;
    }
    socket_fd = fd;
    handler.sa_handler = &socket_event_handler;
    if (sigfillset(&handler.sa_mask) < 0){
        DNSlog_err("sigfillset Failed");
        return DNSErr;
    }
    handler.sa_flags = 0;
    if (sigaction(SIGIO, &handler, 0) < 0) {
        DNSlog_err("Failed to set the signal action");
        return DNSErr;
    }
    if (fcntl(fd, F_SETOWN, getpid()) < 0) {
        DNSlog_err("Failed to set the socket signals");
        return DNSErr;
    }
    // Set to non-blocking
    if (fcntl(fd, F_SETFL, FNONBLOCK | FASYNC)< 0) {
        DNSlog_err("Failed to set the socket to non-blocking");
        return DNSErr;
    }

    struct sockaddr_in6 sa6;
    sa6.sin6_family = AF_INET6;
    sa6.sin6_addr = in6addr_any;
    sa6.sin6_port = htons(0);
    if(bind(fd, (struct sockaddr*)&sa6, sizeof(sa6)) == -1) {
        DNSlog_err("Socket Binding Failed");
        return DNSErr;
    }
    DNSlog_info("DNS Resolver is ready!!");
    return DNSNoErr;
}


int dnsUDPSocket::sendDNSQuery(const unsigned char* query, size_t len, struct sockaddr_storage* ss)
{
    char addr[256];
    int port = 0;
    struct sockaddr_in6 *sa6 = (struct sockaddr_in6*) ss;
    inet_ntop(AF_INET6, &sa6->sin6_addr.s6_addr, addr, sizeof(addr));
    port = ntohs(sa6->sin6_port);
    int ret = (int)sendto(fd, query, len, 0, (struct sockaddr *)ss, sizeof(struct sockaddr_in6));
    if (ret < 0) {
        DNSlog_err("Sendto failed");
        return ret;
    }
    DNSlog_info("Sent %d bytes to [%s:%d]", ret, addr, port);
    return ret;
}
