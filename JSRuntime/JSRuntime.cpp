//
//  JSRuntime.cpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#include <objc/runtime.h>

#include "JSWrapper.hpp"

#include "CUtils.cpp"
#include "CWrapper.hpp"
#include "FoundationWrapper.hpp"



#include ".JSRuntime.coffee"

extern "C" JSValueRef/*exception*/ JSRuntimeAttachToJSContext(JSGlobalContextRef ctx)
{
    JSValueRef exception = NULL;
    
    std::map<const char*, JSValueRef> bindings = {
        {"CUtils", JSObjectMakeConstructor(ctx, NULL, jsr::cutils::ConstructCUtils)},
        {"C", JSObjectMakeConstructor(ctx, NULL, jsr::c::ConstructC)},
        {"Foundation", JSObjectMakeConstructor(ctx, NULL, jsr::foundation::ConstructFoundation)},
    };
    
    std::vector<JSStringRef> fNames;
    std::vector<JSValueRef> fArgs;
    
    for (auto iter = bindings.begin(); iter != bindings.end(); iter++) {
        fNames.push_back(JSStringCreateWithUTF8CString(iter->first));
        fArgs.push_back(iter->second);
    }
    
    JSStringRef initScript = JSStringCreateWithUTF8CString((char*)__buildin__);
    JSStringRef initURL = JSStringCreateWithUTF8CString("JSRuntime");
    
    JSObjectRef fn = JSObjectMakeFunction(ctx, NULL, (unsigned)fNames.size(), fNames.data(), initScript, initURL, 0, &exception);
    if (exception == NULL) JSObjectCallAsFunction(ctx, fn, NULL, fArgs.size(), fArgs.data(), &exception);
    
    return exception;
}