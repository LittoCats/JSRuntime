//
//  JSWrapper.hpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#ifndef JSWrapper_hpp
#define JSWrapper_hpp

#include "Utils.hpp"
#include <ffi.h>

namespace jsr {
    
    class Argument {
    
    public:
        
        struct AType: public ffi_type {
            std::vector<ffi_type*> m_f;
            std::vector<AType*> m_field;
            std::string m_name;
            const char m_encode;
            
            // cann't use this method by your self.
            AType(const ffi_type* type, const char encode);
            
            // enLen 表示 encode 已解析的长度
            AType(const char* encode, int* enLen);
            ~AType();
            
            static const char SupportedEncodeTable[128];
        };
        
    public:
        char m_val[16];
        void(*m_free)(char m_val[16]) = NULL;
        const AType* m_type;
        
    private:
        // 不可复制
        Argument(Argument &) {};
    public:
        Argument(AType const* type): m_type(type) {}
        Argument(JSContextRef ctx, JSValueRef val, AType const* type); /* JSValueCastException*/
        
        ~Argument() {
            if (m_free == NULL) return;
            m_free(m_val);
        }
        
    public:
        JSValueRef getResult(JSContextRef ctx); /* JSValueCastException*/
    };
    
    class Invocation {
    private:
        ffi_cif m_cif;
        std::vector<Argument::AType*> m_types;
        
        // 用于存储自定义类型
        std::vector<Argument::AType*> m_stypes;
        
    private:
        // 不可复制
        Invocation(Invocation& invo) {};
        
    public:
        Invocation(const char* encode);
        ~Invocation();
        
        // follow create rule
        Argument* invok(void* fn, std::vector<Argument*> argv);
        
    public:
        const Argument::AType* getRType() const { return m_types[0]; }
        const Argument::AType* getAType(size_t index) const{ return m_types[index+1]; }
    };
}
#endif /* JSWrapper_hpp */
