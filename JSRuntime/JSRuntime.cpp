//
//  JSRuntime.cpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#include "JSWrapper.hpp"
#include "CWrapper.hpp"
#include <objc/runtime.h>

#include ".JSRuntime.coffee"

extern "C" JSValueRef/*exception*/ JSRuntimeAttachToJSContext(JSGlobalContextRef ctx)
{
    JSValueRef exception = NULL;
    
    std::map<const char*, JSValueRef> bindings = {
        {"_C", JSObjectMakeConstructor(ctx, NULL, jsr::c::ConstructC)}
    };
    
    std::vector<JSStringRef> fNames;
    std::vector<JSValueRef> fArgs;
    
    for (auto iter = bindings.begin(); iter != bindings.end(); iter++) {
        fNames.push_back(JSStringCreateWithUTF8CString(iter->first));
        fArgs.push_back(iter->second);
    }
    
    JSStringRef initScript = JSStringCreateWithUTF8CString(__buildin__);
    JSStringRef initURL = JSStringCreateWithUTF8CString("JSRuntime");
    
    JSObjectRef fn = JSObjectMakeFunction(ctx, NULL, (unsigned)fNames.size(), fNames.data(), initScript, initURL, 0, &exception);
    if (exception == NULL) JSObjectCallAsFunction(ctx, fn, NULL, fArgs.size(), fArgs.data(), &exception);
    
    return exception;
}