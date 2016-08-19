//
//  FoundationWrapper.mm
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#include "FoundationWrapper.hpp"

#include <objc/runtime.h>
#include <objc/message.h>

static JSClassRef JSRNSClass;

static __strong NSMapTable* JSRTable;

#pragma mark - internal

static JSValueRef CallAsFunction(MsgSend)
{
    /**
     *
     */
    
    std::string fnName;
    std::vector<JSValueRef> argv;
    id reciever = nil;
    SEL selector = nil;
    std::string encode;
    
    jsr::Invocation* invocation = NULL;
    std::vector<jsr::Argument*> xargs;
    
    jsr::Argument* result = NULL;
    
    JSValueRef retVal = NULL;
    
    try {
        {
            // 解析reciever
            reciever = [JSRTable objectForKey:@((ptrdiff_t)thisObject)];
            if (reciever == nil) {
                *exception = JSErrorMake(ctx, "Can't send message to nil object.");
                throw exception;
            }
        }
        {
            // 获取 function name
            static JSStringRef fnNameId = JSStringCreateWithUTF8CString("name");
            JSValueRef fnNameVal = JSObjectGetProperty(ctx, function, fnNameId, exception);
            if (*exception != NULL) throw exception;
            fnName = JSValueCast<char*>(ctx, fnNameVal);
        }
        if (argumentCount > 0) {
            if (!JSValueIsObject(ctx, arguments[0])) {
                *exception = JSErrorMake(ctx, "Invalid arguments format .");
                throw exception;
            }
            JSObjectRef object = (JSObjectRef)arguments[0];
            // 解析 selector 及 参数列表
            JSPropertyNameArrayRef nameArray = JSObjectCopyPropertyNames(ctx, object);
            size_t count = JSPropertyNameArrayGetCount(nameArray);
            
            char nameBuffer[32]; // 应该没有长度超过 32 的参数名吧
            for (auto index = 0; index < count; index++) {
                JSStringRef name = JSPropertyNameArrayGetNameAtIndex(nameArray, index);
                JSStringGetUTF8CString(name, nameBuffer, 32);
                
                // 如果是第一个参数，则需要将参数名首字母大写
                // 以 $ 开头的参数名，直接忽略参数名，只保留 ':'
                if (index == 0) nameBuffer[0] = toupper(nameBuffer[0]);
                if (nameBuffer[0] != '$') {
                    fnName += nameBuffer;
                }
                fnName += ':';
                
                //获取参数
                argv.push_back(JSObjectGetProperty(ctx, object, name, NULL));
            }
            
            JSPropertyNameArrayRelease(nameArray);
        }{
            SEL sel = sel_getUid(fnName.c_str());
            Method method = object_isClass(reciever) ? class_getClassMethod(reciever, sel) : class_getInstanceMethod([reciever class], sel);
            
            if (method == NULL) {
                *exception = JSErrorMake(ctx, (std::string("method not found: ") + fnName + "\n" + [reciever description].UTF8String).c_str());
                throw (exception);
            }
            selector = sel;
            const char* description = method_getTypeEncoding(method);
            
            char* en = (char*)description;
            while (*en) {
                if (jsr::Argument::AType::SupportedEncodeTable[*en] == *en) encode += *en;
                en++;
            }
        }{
            invocation = new jsr::Invocation(encode.c_str());
            // 准备参数
            {   // 1. reciever
                jsr::Argument* rec = new jsr::Argument(invocation->getAType(0));
                *((CFTypeRef*)rec->m_val) = (__bridge CFTypeRef)reciever; // 因为是同步调用，这里不再 retain
                xargs.push_back(rec);
                
                // 2. sel
                jsr::Argument* sel = new jsr::Argument(invocation->getAType(1));
                *((SEL*)sel->m_val) = selector;
                xargs.push_back(sel);
            }{
                // 3. 其它参数
                for (auto index = 0; index < argv.size(); index++) {
                    jsr::Argument* arg = new jsr::Argument(ctx, argv[index], invocation->getAType(index+2));
                    xargs.push_back(arg);
                }
            }
            
            result = invocation->invok((void*)objc_msgSend, xargs);
        }
    } catch (JSValueCastException e) {
        *exception = e.exception;
    } catch (JSValueRef* e) {
        *exception = *e;
    }
    
    if (result != NULL) retVal = result->getResult(ctx);
    
    // 释放内存
    if (result != NULL) delete result;
    if (invocation != NULL) delete invocation;
    for (auto iter = xargs.begin(); iter != xargs.end(); iter++) {
        delete *iter;
    }
    
    return retVal;
}

static JSValueRef CallAsFunction(ToString)
{
    // 解析reciever
    id reciever = [JSRTable objectForKey:@((ptrdiff_t)thisObject)];
    return JSValueMake(ctx, [reciever description].UTF8String);
}
static JSValueRef GetProperty(Accessor)
{
    if (JSStringIsEqualToUTF8CString(propertyName, "toString")) return NULL;
    return JSObjectMakeFunctionWithCallback(ctx, propertyName, CallAsMsgSend);
}

static void Finalize(NSObject)
{
    [JSRTable removeObjectForKey:@((ptrdiff_t(object)))];
}

static JSValueRef GetProperty(FoundationClass)
{
    size_t len = JSStringGetLength(propertyName);
    char* name = new char[len + 1];
    JSStringGetUTF8CString(propertyName, name, len + 1);
    
    Class NSClass = objc_getClass(name);
    
    delete [] name;
    name = nullptr;
    
    if (NSClass == nil) return NULL;
    
    JSGlobalContextRef gctx = JSContextGetGlobalContext(ctx);
    ptrdiff_t akey = (ptrdiff_t)gctx + 0x7461;
    
    // 查找 Associate Object
    NSNumber* flag = objc_getAssociatedObject(NSClass, (void*)akey);
    if ([[JSRTable objectForKey:flag] isEqual:NSClass]) {
        return (JSObjectRef)[flag longLongValue];
    }else{
        JSObjectRef JSObject = JSObjectMake(ctx, JSRNSClass, NULL);
        [JSRTable setObject:NSClass forKey:@((ptrdiff_t)JSObject)];
        
        flag = @((ptrdiff_t)JSObject);
        objc_setAssociatedObject(NSClass, (void*)akey, flag, OBJC_ASSOCIATION_RETAIN);
        
        return JSObject;
    }
}

#pragma mark - External Api

void jsr::foundation::ExtractValue(jsr::Argument *argument, JSContextRef ctx, JSObjectRef val)
{
    id object = [JSRTable objectForKey:@((ptrdiff_t)val)];
    
    *((CFTypeRef*)argument->m_val) = (__bridge CFTypeRef)object; // 同步操作，object 还不会被释放，不需要对其 retain
}

JSValueRef jsr::foundation::GenerateValue(jsr::Argument *argument, JSContextRef ctx)
{
    id object = (__bridge id)*((CFTypeRef*)argument->m_val);
    if (object == nil) return NULL;
    
    JSGlobalContextRef gctx = JSContextGetGlobalContext(ctx);
    ptrdiff_t akey = (ptrdiff_t)gctx + 0x7461;
    
    // 查找 Associate Object
    NSNumber* flag = objc_getAssociatedObject(object, (void*)akey);
    if ([[JSRTable objectForKey:flag] isEqual:object]) {
        return (JSObjectRef)[flag longLongValue];
    }else{
        JSObjectRef JSObject = JSObjectMake(ctx, JSRNSClass, NULL);
        [JSRTable setObject:object forKey:@((ptrdiff_t)JSObject)];
        
        flag = @((ptrdiff_t)JSObject);
        objc_setAssociatedObject(object, (void*)akey, flag, OBJC_ASSOCIATION_RETAIN);
        
        return JSObject;
    }
}

JSObjectRef jsr::foundation::CallAsConstructor(Foundation)
{
    if (JSRTable == nil) { JSRTable = [NSMapTable strongToStrongObjectsMapTable]; }
    
    JSGlobalContextRef gctx = JSContextGetGlobalContext(ctx);
    
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.className = "Foundation";
    definition.getProperty = GetFoundationClass;
    JSClassRef Foundation = JSClassCreate(&definition);
    
    JSObjectRef foundation = JSObjectMake(gctx, Foundation, NULL);
    
    JSClassRelease(Foundation);
    
    // 如果 JSRNSClass
    if (JSRNSClass == nil) {
        JSClassDefinition definition = kJSClassDefinitionEmpty;
        definition.className = "NSObject";
        definition.getProperty = GetAccessor;
        definition.finalize = NSObjectFinalize;
        
        JSStaticFunction static_funcs[] = {
            {"toString", CallAsToString, kJSPropertyAttributeReadOnly},
            {NULL, NULL, 0}
        };
        
        definition.staticFunctions = static_funcs;
        
        JSRNSClass = JSClassCreate(&definition);
    }
    
    return foundation;
}