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

template <int T>
class JSWrapper {
public:
    static constexpr ptrdiff_t ISA = T;
    
public:
    const ptrdiff_t isa = (ptrdiff_t)&ISA;
    char m_val[T];
};

template<int T>
constexpr ptrdiff_t JSWrapper<T>::ISA;
    
    
    
    class Argument {
    
    public:
        
        struct AType: public ffi_type {
            std::vector<ffi_type*> m_f;
            std::vector<AType*> m_field;
            std::string m_name;
            
            // enLen 表示 encode 已解析的长度
            AType(const char* encode, int* enLen);
            ~AType();
        };
        
    public:
        char m_val[16];
        void(*m_free)(char m_val[16]) = NULL;
        const ffi_type* m_type;
        
    private:
        // 不可复制
        Argument(Argument &) {};
    public:
        Argument(ffi_type const* type): m_type(type) {}
        Argument(JSContextRef ctx, JSValueRef val, ffi_type const* type); /* JSValueCastException*/
        
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
        std::vector<ffi_type*> m_types;
        
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
        const ffi_type* getRType() const { return m_types[0]; }
        const ffi_type* getAType(size_t index) const{ return m_types[index+1]; }
    };
}
#endif /* JSWrapper_hpp */
