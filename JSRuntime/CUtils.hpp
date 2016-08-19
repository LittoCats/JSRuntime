//
//  CUtils.hpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/18/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//
//  CUtils 主要提供额外的内存操作，从给定的内存地址中读取及写入指定格式的数据
//
//  read write 函数的第一个参数(Number)必需为有效的内存地址，且不进行有效性检查

#ifndef CUtils_hpp
#define CUtils_hpp

#include "Utils.hpp"

namespace jsr { namespace cutils {
    
    JSObjectRef CallAsConstructor(CUtils);
    
}}

#endif /* CUtils_hpp */
