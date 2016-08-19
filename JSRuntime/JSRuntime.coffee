# 这是一个自编译文件
#@COMPILER
fs = require 'fs'
coffeescript = require 'coffee-script'

check = -> try fs.lstatSync(process.argv[1]).mtime.getTime() > fs.lstatSync(process.argv[1].replace(/\/(?=[^\/]+$)/, '/.')).mtime.getTime() catch then false

readSrc = (cb)->cb fs.readFileSync(process.argv[1]).toString().replace /#@COMPILER[\s\S]+#@END/, ''
compile = (cb)-> (src)-> cb coffeescript.compile src
wrapper = (cb)-> (src)-> cb "static const unsigned char __buildin__[] = {#{new Buffer(src).join ','}, 0x00};"
rewrite = (cb)-> (src)->cb fs.writeFileSync process.argv[1].replace(/\/(?=[^\/]+$)/, '/.'), src


if do check then readSrc compile wrapper rewrite (res)-> console.log 'compile JSRuntime.coffee done.'
return
#@END

# (_C, _CUtils)->

###
_CUtils 目前只提供了两个方法， read write 方法

@param addr Number 指针地址
@param type Number 读取的数据类型

如果 type 不存在，则表示读取字符串，需提供以下参数
@param length Number 读取的长度，Optional for utf8, Default: undefined
@param encode Number 源数据格式，Optional, Default: utf8(0)
  只有 encode 为 utf8 时，length 可以缺省，缺省时，一直读取到第一个 0x00
  其它情况下，默廖 length 为 0
read = (addr[, type[, length[, encode]]])->


write = (addr, content[, type,[, length[, encode]]])->

###

# c data type
CUtilsType = {
  '*': 42     # pointer
  'o': 111    # int8_t
  'O': 79     # uint8_t
  'h': 104    # int16_t
  'H': 72     # uint16_t
  't': 116    # int32_t
  'T': 84     # uint32_t
  'x': 120    # int64_t
  'X': 88     # uint64
  'c': 99     # char
  'C': 67     # uchar
  's': 115    # short
  'S': 83     # ushort
  'i': 105    # int
  'I': 73     # uint
  'l': 108    # long
  'L': 76     # ulong
  'q': 113    # longlong
  'Q': 81     # ulonglong
  'f': 102    # float
  'd': 100    # double
  'B': 66     # bool
}

CUtilsEncode = {
  'A':      65
  'ascii':  65
  'B':      66
  'binary': 66
  'H':      72
  'hex':    72
  '6':      54
  'base64': 54
}

###

###

###
Foundation
###

#define _C_SINT8    'o'
#define _C_UINT8    'O'
#define _C_SINT16   'h'
#define _C_UINT16   'H'
#define _C_SINT32   't'
#define _C_UINT32   'T'
#define _C_SINT64   'x'
#define _C_UINT64   'X'
#define _C_ID       '@'
#define _C_CLASS    '#'
#define _C_SEL      ':'
#define _C_CHR      'c'
#define _C_UCHR     'C'
#define _C_SHT      's'
#define _C_USHT     'S'
#define _C_INT      'i'
#define _C_UINT     'I'
#define _C_LNG      'l'
#define _C_ULNG     'L'
#define _C_LNG_LNG  'q'
#define _C_ULNG_LNG 'Q'
#define _C_FLT      'f'
#define _C_DBL      'd'
#define _C_BFLD     'b'
#define _C_BOOL     'B'
#define _C_VOID     'v'
#define _C_UNDEF    '?'
#define _C_PTR      '^'
#define _C_CHARPTR  '*'
#define _C_ATOM     '%'
#define _C_ARY_B    '['
#define _C_ARY_E    ']'
#define _C_UNION_B  '('
#define _C_UNION_E  ')'
#define _C_STRUCT_B '{'
#define _C_STRUCT_E '}'
#define _C_VECTOR   '!'
#define _C_CONST    'r'

do ->
  this.C = do (Meta = C)->
    class C
    C:: = new Meta
    new C
  this.CUtils = do (Meta = CUtils)->
    class CUtils
    CUtils:: = new Meta
    CUtils::Type = CUtilsType
    CUtils::Encode = CUtilsEncode
    new CUtils
  this.Foundation = do (Meta = Foundation)->
    class Foundation
    Foundation:: = new Meta
    new Foundation

# test

do ->
  console.log JSON.stringify this.CUtils.Type, null, 2
  console.log JSON.stringify this.CUtils.Encode, null, 2

  console.log  this.C.printf 'i*c', "I'm message from JSRuntime: %c\n", 97

  console.log handle = this.C.dlopen '**i', 0, 0x8
  console.log dlopen = this.C.dlsym '***', handle,'dlopen'
  console.log this.CUtils.read dlopen, 120
  
  console.log this.Foundation.UIView.alloc().init()
  console.log this.Foundation.NSString.stringWith(UTF8String: "I'm UTF8 string from JavaScript.")
