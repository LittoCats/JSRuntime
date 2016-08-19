//
//  Utils.hpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <chrono>

#include <JavaScriptCore/JavaScript.h>

static char HexEncodeTable[] = "0123456789abcdef";
static char HexDecodeTable[] = {
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0xA,0xB,0xC,0xD,0xE,0xF,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0xa,0xb,0xc,0xd,0xe,0xf,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

static char Base64EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char Base64DecodeTable[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};

#pragma mark - JSValueMake
inline JSValueRef JSValueMake(JSContextRef ctx) { return JSValueMakeUndefined(ctx); }

template<typename T>
inline JSValueRef JSValueMake(JSContextRef ctx, T t) { return JSValueMakeNumber(ctx, t); }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, JSValueRef t) { return t; }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, JSObjectRef t) { return t; }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, bool tf) { return JSValueMakeBoolean(ctx, tf); }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, const void* ptr) { return JSValueMakeNumber(ctx, (ptrdiff_t)ptr); }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, void* ptr) { return JSValueMakeNumber(ctx, (ptrdiff_t)ptr); }

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, const char* cstr) {
    JSStringRef jstr = JSStringCreateWithUTF8CString(cstr);
    JSValueRef jval = JSValueMakeString(ctx, jstr);
    JSStringRelease(jstr);
    return jval;
}

template<>
inline JSValueRef JSValueMake(JSContextRef ctx, char* cstr) {
    JSStringRef jstr = JSStringCreateWithUTF8CString(cstr);
    JSValueRef jval = JSValueMakeString(ctx, jstr);
    JSStringRelease(jstr);
    return jval;
}

//template<>
//inline JSValueRef JSValueMake(JSContextRef ctx, JSON const& json) {
//    JSStringRef jstr = JSStringCreateWithUTF8CString(json.dump().c_str());
//    JSValueRef jval = JSEvaluateScript(ctx, jstr, NULL, NULL, 0, NULL);
//    JSStringRelease(jstr);
//    if (jval == NULL) jval = JSValueMakeUndefined(ctx);
//    return jval;
//}

inline JSObjectRef JSErrorMake(JSContextRef ctx, const char* msg) {
    JSValueRef errmsg = JSValueMake(ctx, msg);
    return JSObjectMakeError(ctx, 1, &errmsg, NULL);
}

inline std::ostream& operator<<(std::ostream& o, const JSStringRef str) {
    size_t len = JSStringGetMaximumUTF8CStringSize(str);
    char* buffer = new char[len];
    JSStringGetUTF8CString(str, buffer, len);
    o << buffer;
    delete [] buffer;
    return o;
}

#pragma mark - JSValueCast
struct JSValueCastException {
    JSValueRef exception;
    
    JSValueCastException(JSValueRef exception): exception(exception) {}
};

template<typename T>
inline T JSValueCast(JSContextRef ctx, JSValueRef val)
{
    JSValueRef exception = NULL;
    double num = JSValueToNumber(ctx, val, &exception);
    if (exception != NULL) throw JSValueCastException(exception);
    return num;
}

// should free the return value with delete[]
template<>
inline char* JSValueCast<char*>(JSContextRef ctx, JSValueRef val)
{
    JSValueRef exception = NULL;
    JSStringRef jstr = JSValueToStringCopy(ctx, val, &exception);
    if (exception != NULL) throw JSValueCastException(exception);
    size_t len = JSStringGetMaximumUTF8CStringSize(jstr);
    char* buffer = new char[len];
    JSStringGetUTF8CString(jstr, buffer, len);
    JSStringRelease(jstr);
    return buffer;
}

#define GetProperty(name)       Get ## name(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)

#define SetProperty(name)       Set ## name(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)

#define CallAsFunction(name)    CallAs ## name(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)

#define CallAsConstructor(name)  Construct ## name(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)

#define Finalize(name)          name ## Finalize(JSObjectRef object)

#define HasInstance(T)          InstanceOf ## T(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception)

#define ConvertToType(T)        T ## ConvertToType(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception)

#endif /* Utils_hpp */
