//
//  Tests.m
//  Tests
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#import <XCTest/XCTest.h>

#import <JavaScriptCore/JavaScriptCore.h>

#import <objc/runtime.h>

extern "C" JSValueRef/*exception*/ JSRuntimeAttachToJSContext(JSGlobalContextRef ctx);


typedef void (^Task)(int fd);

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
    [self getLongDouble:1];
}

- (void)getLongDouble:(long double)d {
    
}

- (void)block:(void(^)(int, float))task {
    
}

- (void)test {
    
    
    
    JSContext* context = [JSContext new];
    context.exceptionHandler = ^(JSContext* context, JSValue* exception) {
        printf("JSException => %s\n", exception.toString.UTF8String);
        if ([exception isObject]) {
            printf("%s\n", [exception.toObject description].UTF8String);
        }
    };
    
    context[@"console"][@"log"] = ^{
        NSArray* argv = [JSContext currentArguments];
        printf("JSConsole => ");
        for (int index = 0; index < argv.count; index++) {
            if (index > 0) printf(", ");
            printf("%s", [argv[index] description].UTF8String);
        }
        printf("\n");
    };
    
    JSValueRef exception = JSRuntimeAttachToJSContext([context JSGlobalContextRef]);
    if (exception) {
        context.exceptionHandler(context, [JSValue valueWithJSValueRef:exception inContext:context]);
    }
}

@end
