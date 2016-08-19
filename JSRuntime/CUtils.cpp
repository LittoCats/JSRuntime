//
//  CUtils.cpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/18/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//
//  

#include "CUtils.hpp"

static JSValueRef CallAsFunction(Read)
{
    /**
     *  读取数据
     *  @param addr Number 指针地址
     *  @param type Number 读取的数据类型, Optional, Default: 'o' (int8_t)
     *  
     *  如果 type 不存在，则表示读取字符串，需提供以下参数
     *  @param encode Number 源数据格式，Optional, Default: utf8(0)
     *  @param length Number 读取的长度，Optional, Default: undefined
     */
    JSValueRef result = NULL;
    
    void*addr = NULL;
    int type = 'o';
    
    try {
        if (argumentCount > 0) addr = (void*)JSValueCast<ptrdiff_t>(ctx, arguments[0]);
        if (argumentCount > 1) type = JSValueCast<int>(ctx, arguments[1]);
        
        if (addr != NULL) {
            switch (type) {
                case '*': /* pointer */         result = JSValueMake(ctx, (double)*((         ptrdiff_t*)addr)); break;
                case 'o': /* int8_t */          result = JSValueMake(ctx, (double)*((            int8_t*)addr)); break;
                case 'O': /* uint8_t */         result = JSValueMake(ctx, (double)*((           uint8_t*)addr)); break;
                case 'h': /* int16_t */         result = JSValueMake(ctx, (double)*((           int16_t*)addr)); break;
                case 'H': /* uint16_t */        result = JSValueMake(ctx, (double)*((          uint16_t*)addr)); break;
                case 't': /* int32_t */         result = JSValueMake(ctx, (double)*((           int32_t*)addr)); break;
                case 'T': /* uint32_t */        result = JSValueMake(ctx, (double)*((          uint32_t*)addr)); break;
                case 'x': /* int64_t */         result = JSValueMake(ctx, (double)*((           int64_t*)addr)); break;
                case 'X': /* uint64 */          result = JSValueMake(ctx, (double)*((          uint64_t*)addr)); break;
                case 'c': /* char */            result = JSValueMake(ctx, (double)*((              char*)addr)); break;
                case 'C': /* uchar */           result = JSValueMake(ctx, (double)*((     unsigned char*)addr)); break;
                case 's': /* short */           result = JSValueMake(ctx, (double)*((             short*)addr)); break;
                case 'S': /* ushort */          result = JSValueMake(ctx, (double)*((    unsigned short*)addr)); break;
                case 'i': /* int */             result = JSValueMake(ctx, (double)*((               int*)addr)); break;
                case 'I': /* uint */            result = JSValueMake(ctx, (double)*((          unsigned*)addr)); break;
                case 'l': /* long */            result = JSValueMake(ctx, (double)*((              long*)addr)); break;
                case 'L': /* ulong */           result = JSValueMake(ctx, (double)*((     unsigned long*)addr)); break;
                case 'q': /* longlong */        result = JSValueMake(ctx, (double)*((         long long*)addr)); break;
                case 'Q': /* ulonglong */       result = JSValueMake(ctx, (double)*((unsigned long long*)addr)); break;
                case 'f': /* float */           result = JSValueMake(ctx, (double)*((             float*)addr)); break;
                case 'd': /* double */          result = JSValueMake(ctx, (double)*((            double*)addr)); break;
                case 'B': /* bool */            result = JSValueMake(ctx, (double)*((              bool*)addr)); break;
                default:    // 默认读取字符串
                    size_t length = 0;
                    char encode = 8; // 默认为 utf8
                    
                    if (argumentCount > 2) encode = JSValueCast<size_t>(ctx, arguments[2]);
                    if (argumentCount > 3) length = JSValueCast<char>(ctx, arguments[3]);
                    
                    switch (encode) {
                        case 'A': // ascii
                        case 'B': {
                            JSChar* jchars = new JSChar[length];
                            for (unsigned char* bytes = (unsigned char*)addr; bytes - (unsigned char*)addr < length; bytes++, jchars++) {
                                *jchars = *bytes;
                            }
                            jchars -= length;
                            JSStringRef jstr = JSStringCreateWithCharacters(jchars, length);
                            result = JSValueMakeString(ctx, jstr);
                            JSStringRelease(jstr);
                            delete [] jchars;
                        }break;
                        case 'H': { // addr 中存储的是 Hex 编码的内容，如果 length == 0, 可一直读到第一个 0x00 结束
                            if (length == 0) while (((char*)addr)[length] != 0x00) length += 1;
                            
                            length = length >> 1 << 1;
                            
                            JSChar* jchars = new JSChar[length/2];
                            for (char* bytes = (char*)addr; bytes - (char*)addr < length; bytes += 2, jchars += 1) {
                                *jchars = ((HexDecodeTable[bytes[0] & 0x7f]) << 4) | (HexDecodeTable[bytes[1] & 0xff]);
                            }
                            jchars -= length/2;
                            JSStringRef jstr = JSStringCreateWithCharacters(jchars, length);
                            result = JSValueMakeString(ctx, jstr);
                            JSStringRelease(jstr);
                            delete [] jchars;
                        }break;
                        case '6': { // addr 中存储的是 Base64 编码的内容，如果 length == 0, 可一直读到第一个 0x00 结束
                            if (length == 0) while (((char*)addr)[length] != 0x00) length += 1;
                            
                            // 计算 有效的 base64 长度
                            length = length >> 2 << 2;
                            size_t len = (length >> 2) * 3;
                            
                            if (((char*)addr)[length - 1] == '+') len -= 1;
                            if (((char*)addr)[length - 2] == '+') len -= 1;
                            
                            JSChar* jchars = new JSChar[len];
                            
                            auto dnd = jchars + len;
                            auto src = (char*)addr, snd = src + length;
                            
                            for (; src < snd; src += 4, jchars += 3)
                            {
                                uint8_t a = Base64DecodeTable[src[0] & 127];
                                uint8_t b = Base64DecodeTable[src[1] & 127];
                                uint8_t c = Base64DecodeTable[src[2] & 127];
                                uint8_t d = Base64DecodeTable[src[3] & 127];
                                
                                uint8_t buffer[] = {
                                    static_cast<uint8_t>((a << 2) | (b >> 4 & 0b11)),
                                    static_cast<uint8_t>((b << 4) | (c >> 2 & 0b1111)),
                                    static_cast<uint8_t>((c << 6) | (d & 0b111111))
                                };
                                
                                jchars[0] = buffer[0];
                                jchars[1] = buffer[1];
                                jchars[2] = buffer[2];
                            }{
                                uint8_t a = Base64DecodeTable[src[0] & 127];
                                uint8_t b = Base64DecodeTable[src[1] & 127];
                                uint8_t c = Base64DecodeTable[src[2] & 127];
                                uint8_t d = Base64DecodeTable[src[3] & 127];
                                
                                uint8_t buffer[] = {
                                    static_cast<uint8_t>((a << 2) | (b >> 4 & 0b11)),
                                    static_cast<uint8_t>((b << 4) | (c >> 2 & 0b1111)),
                                    static_cast<uint8_t>((c << 6) | (d & 0b111111))
                                };
                                
                                if (jchars++ < dnd) jchars[0] = buffer[0];
                                if (jchars++ < dnd) jchars[1] = buffer[1];
                                if (jchars++ < dnd) jchars[2] = buffer[2];
                            }
                            
                            jchars -= len;
                            JSStringRef jstr = JSStringCreateWithCharacters(jchars, length);
                            result = JSValueMakeString(ctx, jstr);
                            JSStringRelease(jstr);
                            delete [] jchars;
                        }break;
                        default: { // addr 中存储的是 utf8 编码的内容，如果 length == 0, 可一直读到第一个 0x00 结束
                            std::string str;
                            if (length == 0) str = std::string((char*)addr);
                            else str = std::string((char*)addr, length);
                            
                            JSStringRef jstr = JSStringCreateWithUTF8CString(str.c_str());
                            result = JSValueMakeString(ctx, jstr);
                            JSStringRelease(jstr);
                        }break;
                    }break;
            }
        }
        
    } catch (JSValueCastException e) {
        *exception = e.exception;
    }
    
    return result;
}

static JSValueRef CallAsFunction(Write)
{
    if (argumentCount < 2) return NULL;
    
    /**
     *  write = (addr, content[, type,[, length[, encode]]])->
     */
    size_t result = 0;
    
    try {
        void*addr = (void*)JSValueCast<ptrdiff_t>(ctx, arguments[0]);
        int type = 'o';
        
        if (argumentCount > 2) type = JSValueCast<int>(ctx, arguments[2]);
        
        switch (type) {
            case '*': /* pointer */   { result = sizeof(         ptrdiff_t); *((         ptrdiff_t*)addr) = JSValueCast<         ptrdiff_t>(ctx, arguments[1]); }break;
            case 'o': /* int8_t */    { result = sizeof(            int8_t); *((            int8_t*)addr) = JSValueCast<            int8_t>(ctx, arguments[1]); }break;
            case 'O': /* uint8_t */   { result = sizeof(           uint8_t); *((           uint8_t*)addr) = JSValueCast<           uint8_t>(ctx, arguments[1]); }break;
            case 'h': /* int16_t */   { result = sizeof(           int16_t); *((           int16_t*)addr) = JSValueCast<           int16_t>(ctx, arguments[1]); }break;
            case 'H': /* uint16_t */  { result = sizeof(          uint16_t); *((          uint16_t*)addr) = JSValueCast<          uint16_t>(ctx, arguments[1]); }break;
            case 't': /* int32_t */   { result = sizeof(           int32_t); *((           int32_t*)addr) = JSValueCast<           int32_t>(ctx, arguments[1]); }break;
            case 'T': /* uint32_t */  { result = sizeof(          uint32_t); *((          uint32_t*)addr) = JSValueCast<          uint32_t>(ctx, arguments[1]); }break;
            case 'x': /* int64_t */   { result = sizeof(           int64_t); *((           int64_t*)addr) = JSValueCast<           int64_t>(ctx, arguments[1]); }break;
            case 'X': /* uint64 */    { result = sizeof(          uint64_t); *((          uint64_t*)addr) = JSValueCast<          uint64_t>(ctx, arguments[1]); }break;
            case 'c': /* char */      { result = sizeof(              char); *((              char*)addr) = JSValueCast<              char>(ctx, arguments[1]); }break;
            case 'C': /* uchar */     { result = sizeof(     unsigned char); *((     unsigned char*)addr) = JSValueCast<     unsigned char>(ctx, arguments[1]); }break;
            case 's': /* short */     { result = sizeof(             short); *((             short*)addr) = JSValueCast<             short>(ctx, arguments[1]); }break;
            case 'S': /* ushort */    { result = sizeof(    unsigned short); *((    unsigned short*)addr) = JSValueCast<    unsigned short>(ctx, arguments[1]); }break;
            case 'i': /* int */       { result = sizeof(               int); *((               int*)addr) = JSValueCast<               int>(ctx, arguments[1]); }break;
            case 'I': /* uint */      { result = sizeof(          unsigned); *((          unsigned*)addr) = JSValueCast<          unsigned>(ctx, arguments[1]); }break;
            case 'l': /* long */      { result = sizeof(              long); *((              long*)addr) = JSValueCast<              long>(ctx, arguments[1]); }break;
            case 'L': /* ulong */     { result = sizeof(     unsigned long); *((     unsigned long*)addr) = JSValueCast<     unsigned long>(ctx, arguments[1]); }break;
            case 'q': /* longlong */  { result = sizeof(         long long); *((         long long*)addr) = JSValueCast<         long long>(ctx, arguments[1]); }break;
            case 'Q': /* ulonglong */ { result = sizeof(unsigned long long); *((unsigned long long*)addr) = JSValueCast<unsigned long long>(ctx, arguments[1]); }break;
            case 'f': /* float */     { result = sizeof(             float); *((             float*)addr) = JSValueCast<             float>(ctx, arguments[1]); }break;
            case 'd': /* double */    { result = sizeof(            double); *((            double*)addr) = JSValueCast<            double>(ctx, arguments[1]); }break;
            case 'B': /* bool */      { result = sizeof(              bool); *((              bool*)addr) = JSValueCast<              bool>(ctx, arguments[1]); }break;
            default: {     // write string
                size_t length = 0;
                char encode = 8; // 默认为 utf8
                
                if (argumentCount > 3) encode = JSValueCast<size_t>(ctx, arguments[3]);
                if (argumentCount > 4) length = JSValueCast<char>(ctx, arguments[4]);
                
                JSStringRef jstr = JSValueToStringCopy(ctx, arguments[1], exception);
                if (*exception == NULL) {
                    switch (encode) {
                        case 'A':
                        case 'B': {
                            const JSChar* jschars = JSStringGetCharactersPtr(jstr);
                            length = std::min(length, JSStringGetLength(jstr));
                            
                            JSChar* src = (JSChar*)jschars;
                            char* bytes = (char*)addr;
                            for (JSChar* snd = src + length; src > snd; src += 1, bytes += 1) {
                                *bytes = *src;
                            }
                        }break;
                        case 'H': {
                            const JSChar* jschars = JSStringGetCharactersPtr(jstr);
                            length = std::min(length*2, JSStringGetLength(jstr));
                            length = length >> 1 << 1;
                            
                            JSChar* src = (JSChar*)jschars;
                            char* bytes = (char*)addr;
                            for (JSChar* snd = src + length; src > snd; src += 2, bytes += 1) {
                                *bytes = ((HexDecodeTable[src[0] & 0x7f]) << 4) | (HexDecodeTable[src[1] & 0x7f]);
                            }
                        }break;
                        case '6': {
                            const JSChar* jschars = JSStringGetCharactersPtr(jstr);
                            length = std::min(length/3*4, JSStringGetLength(jstr));
                        }
                        default: {
                            length = JSStringGetUTF8CString(jstr, (char*)addr, length);
                        }break;
                    }
                    JSStringRelease(jstr);
                    result = length;
                }
            }
        }
        
        
    } catch (JSValueCastException e) {
        *exception = e.exception;
    }
    
    return JSValueMake(ctx, result);
}


JSObjectRef jsr::cutils::CallAsConstructor(CUtils)
{
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.className = "CUtils";
    
    JSStaticFunction static_funcs[] = {
        {"read", CallAsRead, kJSPropertyAttributeReadOnly},
        {"write", CallAsWrite, kJSPropertyAttributeReadOnly},
        {NULL, NULL, 0}
    };
    definition.staticFunctions = static_funcs;
    
    JSClassRef JSClass = JSClassCreate(&definition);
    JSObjectRef cutils = JSObjectMake(ctx, JSClass, NULL);
    JSClassRelease(JSClass);
    
    return cutils;
}