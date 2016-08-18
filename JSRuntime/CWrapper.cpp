//
//  CWrapper.cpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#include "CWrapper.hpp"
#include <dlfcn.h>

#include <ffi.h>



static JSValueRef CallAsFunction(CFunction)
{
    /**
     *  第一个参数为 String , c 方法的签名，详看 JSRuntime.coffee
     *  其余参数为 传参
     */
    void* fn = NULL;
    {
        static JSStringRef nameSig = JSStringCreateWithUTF8CString("name");
        JSValueRef nameVal = JSObjectGetProperty(ctx, function, nameSig, exception);
        if (*exception != NULL) return NULL;
        
        JSStringRef fnVal = JSValueToStringCopy(ctx, nameVal, NULL);
        size_t len = JSStringGetMaximumUTF8CStringSize(fnVal);
        char* fnname = new char[len];
        JSStringGetUTF8CString(fnVal, fnname, len);
        
        fn = (void*)std::stoll(fnname);
    }
    
    jsr::Invocation* invocation = NULL;
    std::vector<jsr::Argument*> argv;
    jsr::Argument* rval = NULL;
    JSValueRef result = NULL;
    
    try {
        // 解析方法签名
        char* encode = JSValueCast<char*>(ctx, arguments[0]);
        invocation = new jsr::Invocation(encode);
        
        // 准备参数
        for (size_t index = 1; index < argumentCount; index++) {
            argv.push_back(new jsr::Argument(ctx, arguments[index], invocation->getAType(index-1)));
        }
        
        rval = invocation->invok(fn, argv);
    } catch (JSValueCastException e) {
        *exception = e.exception;
    }
    
    if (invocation) delete invocation;
    for (auto iter = argv.begin(); iter != argv.end(); iter++) {
        delete *iter;
    }
    if (rval) {
        result = rval->getResult(ctx);
        delete rval;
    }
    
    
    return result;
}

static JSValueRef GetProperty(CFunction)
{
    /**
     *
     */
    JSObjectRef func = NULL;
    
    size_t len = JSStringGetMaximumUTF8CStringSize(propertyName);
    char* name = new char[len];
    JSStringGetUTF8CString(propertyName, name, len);
    
    void* fn = dlsym(JSObjectGetPrivate(object), name);
    
    if (fn != NULL)
    {
        JSStringRef fname = JSStringCreateWithUTF8CString(std::to_string((long long)fn).c_str());
        func = JSObjectMakeFunctionWithCallback(ctx, fname, CallAsCFunction);
        JSStringRelease(fname);
    }
    
    delete [] name;
    
    return func;
}

namespace jsr { namespace c {
    
    JSObjectRef CallAsConstructor(C)
    {
        JSClassDefinition definition = kJSClassDefinitionEmpty;
        definition.className = "C";
        definition.getProperty = GetCFunction;
        
        JSClassRef JSClass = JSClassCreate(&definition);
        
        JSObjectRef C = JSObjectMake(ctx, JSClass, dlopen(NULL, RTLD_GLOBAL));
        
        JSClassRelease(JSClass);
        
        return C;
    }

}}