//
//  CWrapper.hpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//
//  CWrapper 包含 CMemory(void*) 和 基本数据类型 C<Number> 及 CFunction (void(*)())

#ifndef CWrapper_hpp
#define CWrapper_hpp

#include "JSWrapper.hpp"

namespace jsr { namespace c {
    
    JSObjectRef CallAsConstructor(C);
    
}}

#endif /* CWrapper_hpp */
