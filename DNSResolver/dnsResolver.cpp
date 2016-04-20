//
//  dnsResolver.cpp
//  DNSResolver
//
//  Created by Sanjay Patil on 4/19/16.
//

#include "dnsResolver.hpp"
#include <vector>
dnsResolver::dnsResolver()
{
    dnsForwarder = new dnsUDPSocket();
    dnsForwarder->bindDNSSocket();
    dns_response_func = NULL;
}

dnsResolver::~dnsResolver()
{
    dnsForwarder = NULL;
    dns_response_func = NULL;
}

DNSERR dnsResolver::resolveHostName_WithServer(char*host, const char* dnsserver, dns_callback_func callback, int qtype)
{
    dns_response_func = callback;
    int length = 0;
    const unsigned char * ipv4_query = buildDNSQuery(host, qtype, STANDARD_QUERY, &length);
    // Send the DNS query to DNS Server:
    struct sockaddr_storage ss;
    struct sockaddr_in6 *sa6 = (struct sockaddr_in6*) &ss;
    sa6->sin6_family = AF_INET6;
    sa6->sin6_port = htons(53);
    if(inet_pton(AF_INET6, dnsserver, &sa6->sin6_addr.s6_addr) != 1) {
        return DNSErr;
    }
    dnsForwarder->sendDNSQuery(ipv4_query, length, &ss);
    return DNSNoErr;
}

void dnsResolver::buildDNSQueryHeader(unsigned char * dns_query, unsigned int type, u_int8_t opcode, int *length) {
    // Add the 16 bit ID [0x0001] in the query packet
    dns_query[0] = 0x00;
    dns_query[1] = 0x01;
    switch (opcode) {
        case STANDARD_QUERY:
            dns_query[2] = 0x01;
            break;
        case INVERSE_QUERY:
            dns_query[2] = 0x91;
            break;
        default:
            break;
    }
    // QDCount
    dns_query[4] = 0x00;
    dns_query[5] = 0x01;
    // ANCOUNT
    dns_query[6] = 0x00;
    dns_query[7] = 0x00;
    // NSCOUNT
    dns_query[8] = 0x00;
    dns_query[9] = 0x00;
    // ARCOUNT
    dns_query[10] = 0x00;
    dns_query[11] = 0x00;
}
const unsigned char * dnsResolver::buildDNSQuery(char* host, unsigned int type, u_int8_t opcode, int *length)
{
    static unsigned char dns_query[1024]; // TODO:How to decide this size?
    memset(dns_query, 0, sizeof(dns_query));
    buildDNSQueryHeader(dns_query, type, opcode, length);
    // Split the hostnams by delimiter "."
    const char delim[2] = ".";
    unsigned char* qptr = &dns_query[12];
    const char * name = strtok((char*)host, delim);
    *qptr = strlen(name); qptr++;
    memcpy(qptr, name, strlen(name)); qptr += strlen(name);
    
    while (name != NULL) {
        name = strtok(NULL, delim);
        if(name) {
            *qptr = strlen(name); qptr++;
            memcpy(qptr, name, strlen(name)); qptr += strlen(name);
        }
    }
    *qptr = 0; qptr++; // To indicate the end of question name.
    // QTYPE
    switch (type) {
        case A:
            qptr[0] = 0x00;
            qptr[1] = 0x01;
            break;
        case AAAA:
            qptr[0] = 0x00;
            qptr[1] = 0x1c;
            break;
        default:
            break;
    }
    qptr += 2;
    // QCLASS
    qptr[0] = 0x00;
    qptr[1] = 0x01;
    qptr += 2;
    *length = (int)(qptr - dns_query);
    return dns_query;
}

void parse_dns_header(const unsigned char *p, char *header, int BUF_SIZE, int *num_que, int *num_ans)
{
    snprintf(header, BUF_SIZE, "DNS Response - ID: %02x Flags:%02x ", p[0] << 8 | p[1], p[2] << 8 | p[3]);
    uint16_t QDCount = p[4] << 8 | p[5];
    *num_que = QDCount;
    snprintf(&header[strlen(header)], BUF_SIZE - strlen(header), "QDCOUNT: %d ",QDCount);
    uint16_t ANCount = p[6] << 8 | p[7];
    *num_ans = ANCount;
    snprintf(&header[strlen(header)], BUF_SIZE - strlen(header), "ANCount: %d ",ANCount);
    uint16_t NSCount = p[8] << 8 | p[9];
    snprintf(&header[strlen(header)], BUF_SIZE - strlen(header), "NSCount: %d ",NSCount);
    uint16_t ARCount = p[10] << 8 | p[11];
    snprintf(&header[strlen(header)], BUF_SIZE - strlen(header), "ARCount: %d ",ARCount);
}

const unsigned char * parse_dns_question(const unsigned char *header, const unsigned char *p, char *questions, int BUF_SIZE, int num_que, int *type)
{
    uint8_t label = p[0];
    unsigned char domain[256];
    memset(domain, 0, sizeof(domain));
    int offset = 0;
    while (label) {
        p++;
        // check if the name is a pointer
        if (label & 0xc0) {
            // is a pointer
            int offset = label & 0x3f;
            p = header + offset;
        } else {
            memcpy(&domain[offset], p, label);
            p += label;
            offset += label;
            domain[offset] = '.';
            ++offset;
        }
        label = p[0];
    }
    p++;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "QNAME - %s ", domain);
    int QType = p[0] << 8 | p[1]; p += 2;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "QTYPE - %d ", QType);
    *type = QType;
    int QClass = p[0] << 8 | p[1]; p += 2;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "QCLASS - %d \n", QClass);
    return p;
}

static int ans_offset = 6;

const unsigned char * parse_dns_answers(const unsigned char *header, const unsigned char *p, char *questions, int BUF_SIZE, int type)
{
    uint8_t label = p[0];
    unsigned char domain[256];
    memset(domain, 0, sizeof(domain));
    int offset = 0;
    while (label) {
        p++;
        // check if the name is a pointer
        if (label & 0xc0) {
            // is a pointer
            int offset = (label & 0x3f) << 8 | p[0];
            p = header + offset;
        } else {
            memcpy(&domain[offset], p, label);
            p += label;
            offset += label;
            domain[offset] = '.';
            ++offset;
        }
        label = p[0];
    }
    p++;
    p += ans_offset;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "QNAME - %s ", domain);
    int RRTYPE = p[0] << 8 | p[1]; p += 2;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "RRTYPE - %d ", RRTYPE);
    int RRCLASS = p[0] << 8 | p[1]; p += 2;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "RRCLASS - %d ", RRCLASS);
    int TTL = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]; p += 4;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "TTL - %d ", TTL);
    int RDLENGTH = p[0] << 8 | p[1]; p += 2;
    snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "RDLENGTH - %d ", RDLENGTH);
    static char addr[256];
    if (RDLENGTH == 4) {
        inet_ntop(AF_INET, p, addr, sizeof(addr)); p += 4;
        snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "IP Address - %s \n", addr);
    } else if (RDLENGTH == 16) {
        inet_ntop(AF_INET6, p, addr, sizeof(addr)); p += 16;
        snprintf(&questions[strlen(questions)], BUF_SIZE - strlen(questions), "IP Address - %s \n", addr);
    }
    
    return p;
}

void dnsResolver::parse_dns_response(const unsigned char *response, int err)
{
    int BUF_SIZE = 1024;
    char header[BUF_SIZE];
    const unsigned char *p = response;
    memset(header, 0, sizeof(header));
    int num_que = 0;
    int num_ans = 0;
    int type = 0;
    parse_dns_header(p, header, BUF_SIZE, &num_que, &num_ans);
    const unsigned char * ans_section = parse_dns_question(response, &p[12], header, BUF_SIZE, num_que, &type);
    for (int i = 0; i < num_ans; i++) {
        const unsigned char * p = parse_dns_answers(response, ans_section, header, BUF_SIZE, num_ans);
        if (type == A) {
            ans_offset += (2 + 2 + 4 + 2 + 4 + 2);
        } else if (type == AAAA) {
            ans_offset += (2 + 2 + 4 + 2 + 16 + 2);
        }
        
        ans_section = p;
        
    }
    DNSlog_info("Parsed DNS Response-%s", header);
    dns_response_func(response, err);
}
