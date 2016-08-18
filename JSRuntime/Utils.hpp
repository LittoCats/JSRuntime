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

#define Finalize(name)          Finalize ## name(JSObjectRef object)

#define HasInstance(T)          InstanceOf ## T(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception)

#define ConvertToType(T)        T ## ConvertToType(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception)

#endif /* Utils_hpp */
