//
//  DNSResolverTests.m
//  DNSResolverTests
//
//  Created by Sanjay Patil on 4/19/16.
//

#import <XCTest/XCTest.h>
#import "dnsResolver.hpp"

@interface DNSResolverTests : XCTestCase

@end

@implementation DNSResolverTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

void dns_response(const unsigned char * ip, int err)
{
    NSLog(@"dns_response is of length = %d",err);
}
-(void) testResolveUsingDNSServer{
    dnsResolver *resolver = new dnsResolver();
    char host[256] = "www.google.com";
    resolver->resolveHostName_WithServer(host, "::ffff:10.84.54.20", dns_response, 40);
}

@end
