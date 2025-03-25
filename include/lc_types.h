#ifndef LC_TYPES_H
#define LC_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void        lc_void;

/* Numeric types */

typedef int8_t      lc_int8;
typedef int16_t     lc_int16;
typedef int32_t     lc_int32; 
typedef int64_t     lc_int64;

typedef uint8_t     lc_uint8;
typedef uint16_t    lc_uint16;
typedef uint32_t    lc_uint32; 
typedef uint64_t    lc_uint64;

typedef lc_uint64   lc_usize;
typedef lc_uint8    lc_bool;
typedef char        lc_char;

typedef float       lc_float32; 
typedef double      lc_float64;

typedef enum
{
    LCT_VOID,
    LCT_NULL,
    LCT_BOOL,
    LCT_STRING,
    
    LCT_INTEGER,
    LCT_FLOAT,
    LCT_ARRAY,
    LCT_STRUCT
} lc_type_kind;

typedef struct
{
    lc_uint32   bits;
    lc_bool     sign;
} lc_number;

typedef struct
{
    lc_char*    data;
    lc_usize    size;
} lc_string;

typedef struct
{

} lc_struct;

typedef struct
{
    lc_type_kind    kind; 
    lc_string       idnt; 
    lc_void*        data;
} lc_type;

typedef struct
{
    lc_type*    type;
    lc_void*    data;
    lc_usize    size;
    lc_bool     ptr;
} lc_value;

lc_value*   lc_value_new    (lc_type* type, lc_void* data, lc_usize size, bool ptr);
lc_void     lc_value_dump   (const lc_value* value);

lc_bool     lc_value_toint  ();

#define     lc_string_comptime(str) ((lc_string){ str, (sizeof(str) - 1) })
lc_string*  lc_string_new       (const lc_char* str, lc_usize size);
lc_string*  lc_string_format    (const lc_char* format, ...);
lc_string*  lc_string_dupe      (const lc_string* str);
lc_void     lc_string_free      (lc_string* str);

#endif