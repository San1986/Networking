//
//  dnsResolverConstants.h
//  DNSResolver
//
//  Created by Sanjay Patil on 4/15/16.
//

#include <sys/errno.h>
#include <string.h>

#ifndef dnsResolverConstants_h
#define dnsResolverConstants_h

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define DNSlog_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define DNSlog_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define DNSlog_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define DNSNoErr 0

#define DNSErr -1

#define A       1
#define AAAA    40

#define STANDARD_QUERY 0
#define INVERSE_QUERY  1


#endif /* dnsResolverConstants_h */
