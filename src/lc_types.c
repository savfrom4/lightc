#include "lc_types.h"
#include "lc_error.h"
#include "lc_mem.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

static lc_number STATIC_NUMERIC_DATA[] = {
    {sizeof(lc_int8), true},
    {sizeof(lc_int16), true},
    {sizeof(lc_int32), true},
    {sizeof(lc_int64), true},

    {sizeof(lc_uint8), false},
    {sizeof(lc_uint16), false},
    {sizeof(lc_uint32), false},
    {sizeof(lc_uint64), false},
    {sizeof(lc_usize), false},

    {sizeof(lc_float32), false},
    {sizeof(lc_float64), false},

};

lc_type STATIC_TYPES[] = {
    {LCT_VOID, lc_string_comptime("void"), NULL},
    {LCT_NULL, lc_string_comptime("null"), NULL},
    {LCT_BOOL, lc_string_comptime("bool"), NULL},
    {LCT_STRING, lc_string_comptime("string"), NULL},
    {LCT_LIST, lc_string_comptime("list"), NULL},

    {LCT_INTEGER, lc_string_comptime("int8"), &STATIC_NUMERIC_DATA[0]},
    {LCT_INTEGER, lc_string_comptime("int16"), &STATIC_NUMERIC_DATA[1]},
    {LCT_INTEGER, lc_string_comptime("int32"), &STATIC_NUMERIC_DATA[2]},
    {LCT_INTEGER, lc_string_comptime("int64"), &STATIC_NUMERIC_DATA[3]},
    {LCT_INTEGER, lc_string_comptime("uint8"), &STATIC_NUMERIC_DATA[4]},
    {LCT_INTEGER, lc_string_comptime("uint16"), &STATIC_NUMERIC_DATA[5]},
    {LCT_INTEGER, lc_string_comptime("uint32"), &STATIC_NUMERIC_DATA[6]},
    {LCT_INTEGER, lc_string_comptime("uint64"), &STATIC_NUMERIC_DATA[7]},
    {LCT_INTEGER, lc_string_comptime("usize"), &STATIC_NUMERIC_DATA[8]},

    {LCT_FLOAT, lc_string_comptime("float32"), &STATIC_NUMERIC_DATA[9]},
    {LCT_FLOAT, lc_string_comptime("float64"), &STATIC_NUMERIC_DATA[10]},
};

lc_string *lc_string_new(const lc_char *str, lc_usize size)
{
    lc_string *str_ptr = lc_mem_alloc(sizeof(lc_string));
    if (!str_ptr)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str_ptr");
        return NULL;
    }

    *str_ptr = (lc_string){
        .data = lc_mem_alloc(size + 1),
        .size = size,
    };

    if (!str_ptr->data)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str_ptr->data");
        lc_mem_free(str_ptr);
        return NULL;
    }

    if (str)
        lc_mem_copy(str_ptr->data, str, size);

    return str_ptr;
}

lc_string *lc_string_format(const lc_char *format, ...)
{
    va_list args;
    va_start(args, format);

    // ask for buffer size
    int length = vsnprintf(NULL, 0, format, args);
    if (length < 0)
    {
        lc_error_set(LCE_OTHER, "vsnprintf < 0");
        return NULL;
    }

    // allocate string of said buffer and call vsnprintf
    lc_string *str = lc_string_new(NULL, length);
    if (!str)
    {
        lc_error_set(LCE_ALLOC_FAILED, "str");
        return NULL;
    }

    vsnprintf(str->data, length, format, args);
    va_end(args);
    return str;
}

lc_string *lc_string_dupe(const lc_string *str)
{
    return lc_string_new(str->data, str->size);
}

lc_void lc_string_free(lc_string *str)
{
    lc_mem_free(str->data);
    lc_mem_free(str);
}

lc_list *lc_list_new(lc_usize capacity)
{
    lc_list *list = lc_mem_alloc(sizeof(lc_list));
    if (!list)
    {
        lc_error_set(LCE_ALLOC_FAILED, "list");
        return NULL;
    }

    *list = (lc_list){
        lc_mem_alloc(capacity * sizeof(lc_void *)),
        capacity,
        0,
    };

    if (!list->ptr)
    {
        lc_mem_free(list);
        lc_error_set(LCE_ALLOC_FAILED, "list->ptr");
        return NULL;
    }

    return list;
}

lc_bool lc_list_append(lc_list *list, void *value)
{
    if (!list)
    {
        lc_error_set(LCE_INVALID_ARGUMENT, "list");
        return false;
    }

    if (list->size >= list->capt)
    {
        lc_usize new_capt = list->capt * 2;
        lc_void *new_ptr = lc_mem_realloc(list->ptr, new_capt);

        if (!new_ptr)
        {
            lc_error_set(LCE_ALLOC_FAILED, "new_ptr");
            return false;
        }

        list->ptr = new_ptr;
        list->capt = new_capt;
    }

    list->ptr[list->size++] = value;
    return true;
}

lc_void lc_list_free(lc_list *list)
{
    lc_mem_free(list->ptr);
    lc_mem_free(list);
}

lc_value *lc_value_new(lc_type *type, lc_void *data, lc_usize size, bool ptr)
{
    lc_value *value = lc_mem_alloc(sizeof(lc_value));
    if (!value)
    {
        lc_error_set(LCE_ALLOC_FAILED, "value");
        return NULL;
    }

    *value = (lc_value){
        type,
        data,
        size,
        ptr,
    };

    return value;
}

lc_void lc_value_dump(const lc_value *value)
{
    printf("lc_value { type: %s, data: %p, size: %lu, flags: %d }\n", value->type->idnt.data, value->data, value->size, value->ptr);
}

lc_void lc_value_free(lc_value *value)
{
    lc_mem_free(value->data);
    lc_mem_free(value);
}