//
//  JSWrapper.cpp
//  JSRuntime
//
//  Created by 程巍巍 on 8/17/16.
//  Copyright © 2016 程巍巍. All rights reserved.
//

#include "JSWrapper.hpp"

template <typename T>
static ffi_type* GetIntegerType() {
    size_t len = sizeof(T);
    switch (len) {
        case 1: return &ffi_type_sint8;
        case 2: return &ffi_type_sint16;
        case 4: return &ffi_type_sint32;
        case 8: return &ffi_type_sint64;
    }
    throw std::runtime_error("unsupported type.");
    return &ffi_type_void;
}

template <typename T>
static ffi_type* GetUnIntegerType() {
    size_t len = sizeof(T);
    switch (len) {
        case 1: return &ffi_type_sint8;
        case 2: return &ffi_type_sint16;
        case 4: return &ffi_type_sint32;
        case 8: return &ffi_type_sint64;
    }
    throw std::runtime_error("unsupported type.");
    return &ffi_type_void;
}

#pragma mark - ffi_type Table
// 方法签名 参照 oc 标准
static ffi_type** DFTT /*Default FFI Type Table*/ = []()->ffi_type**{
    static ffi_type*  DFTT[128];
    
    DFTT['o'] = &ffi_type_sint8;        /* #define _C_SINT8 */
    DFTT['O'] = &ffi_type_uint8;        /* #define _C_UINT8 */
    DFTT['h'] = &ffi_type_sint16;       /* #define _C_SINT16 */
    DFTT['H'] = &ffi_type_uint16;       /* #define _C_UINT16 */
    DFTT['t'] = &ffi_type_sint32;       /* #define _C_SINT32 */
    DFTT['T'] = &ffi_type_uint32;       /* #define _C_UINT32 */
    DFTT['x'] = &ffi_type_sint64;       /* #define _C_SINT64 */
    DFTT['X'] = &ffi_type_uint64;       /* #define _C_UINT64 */
    DFTT['@'] = &ffi_type_pointer;      /* #define _C_ID */
    DFTT['#'] = &ffi_type_pointer;      /* #define _C_CLASS */
    DFTT[':'] = &ffi_type_pointer;      /* #define _C_SEL */
    DFTT['c'] = &ffi_type_schar;        /* #define _C_CHR */
    DFTT['C'] = &ffi_type_uchar;        /* #define _C_UCHR */
    DFTT['s'] = &ffi_type_sshort;       /* #define _C_SHT */
    DFTT['S'] = &ffi_type_ushort;       /* #define _C_USHT */
    DFTT['i'] = &ffi_type_sint;         /* #define _C_INT */
    DFTT['I'] = &ffi_type_uint;         /* #define _C_UINT */
    DFTT['l'] = &ffi_type_slong;        /* #define _C_LNG */
    DFTT['L'] = &ffi_type_ulong;        /* #define _C_ULNG */
    DFTT['q'] = GetIntegerType<long long>(); /* #define _C_LNG_LNG */
    DFTT['Q'] = GetUnIntegerType<unsigned long long>(); /* #define _C_ULNG_LNG */
    DFTT['f'] = &ffi_type_float;        /* #define _C_FLT */
    DFTT['d'] = &ffi_type_double;       /* #define _C_DBL */
//    DFTT['b'] = GetIntegerType<bool>(); /* #define _C_BFLD */
    DFTT['B'] = GetIntegerType<bool>(); /* #define _C_BOOL */
    DFTT['v'] = &ffi_type_void;         /* #define _C_VOID */
//    DFTT['?'] = &ffi_type_;           /* #define _C_UNDEF */
    DFTT['^'] = &ffi_type_pointer;      /* #define _C_PTR */
    DFTT['*'] = &ffi_type_pointer;      /* #define _C_CHARPTR */
//    DFTT['%'] = &ffi_type_; /* #define _C_ATOM */
//    DFTT['['] = &ffi_type_; /* #define _C_ARY_B */
//    DFTT[']'] = &ffi_type_; /* #define _C_ARY_E */
//    DFTT['('] = &ffi_type_; /* #define _C_UNION_B */
//    DFTT[')'] = &ffi_type_; /* #define _C_UNION_E */
//    DFTT['{'] = &ffi_type_; /* #define _C_STRUCT_B */
//    DFTT['}'] = &ffi_type_; /* #define _C_STRUCT_E */
//    DFTT['!'] = &ffi_type_; /* #define _C_VECTOR */
//    DFTT['r'] = &ffi_type_; /* #define _C_CONST */
    return DFTT;
}();

jsr::Invocation::Invocation(const char* encode)
{
    // 1. 解析 encode
    int loc = 0;
    while (encode[loc] != 0x00) {
        int offset = 0;
        ffi_type* t = DFTT[encode[loc]];
        if (t) {
            m_types.push_back(t);
            offset = 1;
        }else if (encode[loc] == '{'){
            // struct
            jsr::Argument::AType* t = new jsr::Argument::AType(encode+loc, &offset);
            m_stypes.push_back(t);
            m_types.push_back((ffi_type*)m_stypes[m_stypes.size() - 1]);
        }else{
            throw std::runtime_error(&"unsupported encode : " [ encode[loc]]);
        }
        loc += offset;
    }
    
    // 2. prepare cif
    if (FFI_OK != ffi_prep_cif(&m_cif, FFI_DEFAULT_ABI,(unsigned) m_types.size() - 1, m_types[0], m_types.data() + 1)) {
        throw std::runtime_error(std::string("Invalid Invocation encode: ") + encode);
    }
}

jsr::Invocation::~Invocation()
{
    // 释放结构体类型
    for (auto iter = m_stypes.begin(); iter != m_stypes.end(); iter++) {
        delete *iter;
    }
}

jsr::Argument* jsr::Invocation::invok(void *fn, std::vector<Argument *> argv)
{
    Argument* rvalue = new Argument(m_types[0]);
    void** avalues = new void*[m_types.size()];
    for (auto index = 0; index < argv.size(); index++) {
        avalues[index] = &argv[index]->m_val;
    }
    ffi_call(&m_cif, (void (*)())fn, &rvalue->m_val, avalues);
    return rvalue;
}

jsr::Argument::Argument(JSContextRef ctx, JSValueRef val, ffi_type const* type): m_type(type)
{
    switch (m_type->type) {
        case FFI_TYPE_VOID: return;
        case FFI_TYPE_INT:      *((int*)m_val) = JSValueCast<int>(ctx, val);            break;
        case FFI_TYPE_FLOAT:    *((float*)m_val) = JSValueCast<float>(ctx, val);        break;
        case FFI_TYPE_DOUBLE:   *((double*)m_val) = JSValueCast<double>(ctx, val);      break;
        case FFI_TYPE_UINT8:    *((uint8_t*)m_val) = JSValueCast<uint8_t>(ctx, val);    break;
        case FFI_TYPE_SINT8:    *((int8_t*)m_val) = JSValueCast<int8_t>(ctx, val);      break;
        case FFI_TYPE_UINT16:   *((uint16_t*)m_val) = JSValueCast<uint16_t>(ctx, val);  break;
        case FFI_TYPE_SINT16:   *((int16_t*)m_val) = JSValueCast<int16_t>(ctx, val);    break;
        case FFI_TYPE_UINT32:   *((uint32_t*)m_val) = JSValueCast<uint32_t>(ctx, val);  break;
        case FFI_TYPE_SINT32:   *((int32_t*)m_val) = JSValueCast<int32_t>(ctx, val);    break;
        case FFI_TYPE_UINT64:   *((uint64_t*)m_val) = JSValueCast<uint64_t>(ctx, val);  break;
        case FFI_TYPE_SINT64:   *((int64_t*)m_val) = JSValueCast<int64_t>(ctx, val);    break;
        case FFI_TYPE_STRUCT:   {
            throw std::runtime_error("struct argument not supported yet.");
        }break;
        case FFI_TYPE_POINTER:  {
            // void* 类型稍微复杂一些，
            // 如果 val 是 String , 将转化为 char*
            // 其它类型，直接调用 toNumber 方法
            if (JSValueIsString(ctx, val)) {
                char* bytes = JSValueCast<char*>(ctx, val);
                *(char**)m_val = bytes;
                
                m_free = [](char val[16]){
                    char* bytes = *(char**)val;
                    delete [] bytes;
                };
            }else{
                *(void**)m_val = (void*)JSValueCast<ptrdiff_t>(ctx, val);
            }
        }break;
        default:
            throw std::runtime_error("unsupported argument type.");
    }
}

JSValueRef jsr::Argument::getResult(JSContextRef ctx)
{
    assert(m_type);
    switch (m_type->type) {
        case FFI_TYPE_VOID:     return NULL;
        case FFI_TYPE_INT:      return JSValueMake<int>(ctx, *((int*)m_val));
        case FFI_TYPE_FLOAT:    return JSValueMake<float>(ctx, *((float*)m_val));
        case FFI_TYPE_DOUBLE:   return JSValueMake<double>(ctx, *((double*)m_val));
        case FFI_TYPE_UINT8:    return JSValueMake<uint8_t>(ctx, *((uint8_t*)m_val));
        case FFI_TYPE_SINT8:    return JSValueMake<int8_t>(ctx, *((int8_t*)m_val));
        case FFI_TYPE_UINT16:   return JSValueMake<uint16_t>(ctx, *((uint16_t*)m_val));
        case FFI_TYPE_SINT16:   return JSValueMake<int16_t>(ctx, *((int16_t*)m_val));
        case FFI_TYPE_UINT32:   return JSValueMake<uint32_t>(ctx, *((uint32_t*)m_val));
        case FFI_TYPE_SINT32:   return JSValueMake<int32_t>(ctx, *((int32_t*)m_val));
        case FFI_TYPE_UINT64:   return JSValueMake<uint64_t>(ctx, *((uint64_t*)m_val));
        case FFI_TYPE_SINT64:   return JSValueMake<int64_t>(ctx, *((int64_t*)m_val));
        case FFI_TYPE_STRUCT:   throw std::runtime_error("struct argument not supported yet.");
        case FFI_TYPE_POINTER:  return JSValueMake<ptrdiff_t>(ctx, *(ptrdiff_t*)m_val);
            
        default:throw std::runtime_error("unsupported argument type.");
    }
    return NULL;
}

jsr::Argument::AType::AType(const char* encode, int* enLen)
{
    *enLen = 1;
    
    // encode = {CGRect={CGPoint=dd}{CGSize=dd}}
    
    // 1. 找到名字
    while (encode[*enLen] != '=') {
        m_name += encode[*enLen];
        *enLen += 1;
    }
    
    // 2. 跳过 '='
    *enLen += 1;
    
    // 3. 解析 elements
    while (encode[*enLen] != '}') {
        int offset = 1;
        ffi_type* t = DFTT[encode[*enLen]];
        if (t != 0x00) {
            m_f.push_back(t);
        }else if(encode[*enLen] == '{') {
            // struct
            jsr::Argument::AType* t = new jsr::Argument::AType(encode + (*enLen), &offset);
            m_field.push_back(t);
            m_f.push_back((ffi_type*)m_field[m_field.size() - 1]);
        }else{
            throw std::runtime_error(std::string("unsupported encode : ") + encode[*enLen]);
        }
        
        *enLen += offset;
    }
    
    // 4. 设置 type elements
    m_f.push_back(NULL);
    this->type = FFI_TYPE_STRUCT;
    this->elements = m_f.data();
    
    // 5. 添加结束标志
    *enLen += 1;
}

jsr::Argument::AType::~AType()
{
    // 释放 m_field 中的象
    for (auto iter = m_field.begin(); iter != m_field.end(); iter++) {
        delete *iter;
    }
}

