# 这是一个自编译文件
#@COMPILER
fs = require 'fs'
coffeescript = require 'coffee-script'

check = -> try fs.lstatSync(process.argv[1]).mtime.getTime() > fs.lstatSync(process.argv[1].replace(/\/(?=[^\/]+$)/, '/.')).mtime.getTime() catch then false

readSrc = (cb)->cb fs.readFileSync(process.argv[1]).toString().replace /#@COMPILER[\s\S]+#@END/, ''
compile = (cb)-> (src)-> cb coffeescript.compile src
wrapper = (cb)-> (src)-> cb "static const char __buildin__[] = {#{new Buffer(src).join ','}, 0x00};"
rewrite = (cb)-> (src)->cb fs.writeFileSync process.argv[1].replace(/\/(?=[^\/]+$)/, '/.'), src


if do check then readSrc compile wrapper rewrite (res)-> console.log 'compile JSRuntime.coffee done.'
return
#@END

# (C)->
C = new _C
console.log  C.printf 'i*i', "I'm message from JSRuntime: %i\n", 1234

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