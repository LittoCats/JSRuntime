//
//  FoundationWrapper.hpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#ifndef FoundationWrapper_hpp
#define FoundationWrapper_hpp


#include "Utils.hpp"

#include "JSWrapper.hpp"

namespace jsr { namespace foundation {
    
    void ExtractValue(jsr::Argument* argument, JSContextRef ctx, JSObjectRef object);
    JSValueRef GenerateValue(jsr::Argument* argument, JSContextRef ctx);
    
    JSObjectRef CallAsConstructor(Foundation);
    
}}



#endif /* FoundationWrapper_hpp */
