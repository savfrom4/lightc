#include "lc_types.h"
#include "lc_error.h"
#include "lc_mem.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

static lc_number STATIC_NUMERIC_DATA[] = {
    { sizeof(lc_int8),      true },
    { sizeof(lc_int16),     true },
    { sizeof(lc_int32),     true },
    { sizeof(lc_int64),     true },

    { sizeof(lc_uint8),     false },
    { sizeof(lc_uint16),    false },
    { sizeof(lc_uint32),    false },
    { sizeof(lc_uint64),    false },
    { sizeof(lc_usize),     false },

    { sizeof(lc_float32),   false },
    { sizeof(lc_float64),   false },

};

static lc_type STATIC_TYPES[] = {
    { LCT_VOID, lc_string_comptime("void"), NULL },
    { LCT_NULL, lc_string_comptime("null"), NULL },
    { LCT_BOOL, lc_string_comptime("bool"), NULL },
    { LCT_STRING, lc_string_comptime("string"), NULL },
    { LCT_ARRAY, lc_string_comptime("array"), NULL },

    { LCT_INTEGER, lc_string_comptime("int8"), &STATIC_NUMERIC_DATA[0] },
    { LCT_INTEGER, lc_string_comptime("int16"), &STATIC_NUMERIC_DATA[1] },
    { LCT_INTEGER, lc_string_comptime("int32"), &STATIC_NUMERIC_DATA[2] },
    { LCT_INTEGER, lc_string_comptime("int64"), &STATIC_NUMERIC_DATA[3] },
    { LCT_INTEGER, lc_string_comptime("uint8"), &STATIC_NUMERIC_DATA[4] },
    { LCT_INTEGER, lc_string_comptime("uint16"), &STATIC_NUMERIC_DATA[5] },
    { LCT_INTEGER, lc_string_comptime("uint32"), &STATIC_NUMERIC_DATA[6] },
    { LCT_INTEGER, lc_string_comptime("uint64"), &STATIC_NUMERIC_DATA[7] },
    { LCT_INTEGER, lc_string_comptime("usize"), &STATIC_NUMERIC_DATA[8] },

    { LCT_FLOAT, lc_string_comptime("float32"), &STATIC_NUMERIC_DATA[9] },
    { LCT_FLOAT, lc_string_comptime("float64"), &STATIC_NUMERIC_DATA[10] },
};

lc_value* lc_value_new(lc_type* type, lc_void* data, lc_usize size, bool ptr)
{
    lc_value* value = lc_mem_alloc(sizeof(lc_value));
    if(!value)
    {
        lc_error_set(LCE_ALLOC_FAILED, "value");
        return NULL;
    }

    *value = (lc_value) {
        .type = type,
        .data = data,
        .size = size,
        .ptr = ptr
    };

    return value;
}

lc_void lc_value_dump(const lc_value* value)
{

}

lc_string* lc_string_new(const lc_char* str, lc_usize size)
{
    lc_string* str_ptr = lc_mem_alloc(sizeof(lc_string));
    if(!str_ptr)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str_ptr");
        return NULL;
    }

    *str_ptr = (lc_string) {
        .data = lc_mem_alloc(size + 1),
        .size = size
    };

    if(!str_ptr->data)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str_ptr->data");
        lc_mem_free(str_ptr);
        return NULL;
    }
    
    if(str)
        lc_mem_cpy(str_ptr->data, str, size);
    
    return str_ptr;
}

lc_string* lc_string_format(const lc_char* format, ...)
{
    va_list args;
    va_start(args, format);

    // ask for buffer size
    int length = vsnprintf(NULL, 0, format, args);
    if(length < 0)
    {
        lc_error_set(LCE_OTHER, "vsnprintf < 0");
        return NULL;
    }

    // allocate string of said buffer and call vsnprintf
    lc_string* str = lc_string_new(NULL, length);
    if(!str)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str");
        return NULL;
    }

    vsnprintf(str->data, length, format, args);
    va_end(args);
    return str;
}

lc_string* lc_string_dupe(const lc_string* str)
{
    return lc_string_new(str->data, str->size);
}

lc_void lc_string_free(lc_string* str)
{
    lc_mem_free(str->data);
    lc_mem_free(str);
}