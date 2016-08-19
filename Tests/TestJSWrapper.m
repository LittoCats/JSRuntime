//
//  TestJSWrapper.m
//  JSRuntime
//
//  Created by 程巍巍 on 8/18/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <objc/runtime.h>

@interface TestJSWrapper : XCTestCase

@end

@implementation TestJSWrapper

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

struct JSBlock_s {
    
} JSBlock;

- (void)test {
    
    void (^block)() = ^{};
    
    id task = block;
    
    Class cls = [task class];
    
    unsigned outCOunt = 0;
    Method* methods = class_copyMethodList(cls, &outCOunt);
    for (unsigned index = 0; index < outCOunt; index++) {
        printf("name: %s\t\n", sel_getName(method_getName(methods[index])));
    }
    
    NSLog(@"done.");
    
}

@end
