#include "lc_types.h"
#include "lc_error.h"
#include "lc_mem.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static lc_string STATIC_TYPE_NAMES[] = {
    lc_string_comptime("void"),
    lc_string_comptime("null"),
    lc_string_comptime("string"),
    lc_string_comptime("list"),
    lc_string_comptime("struct"),
    lc_string_comptime("bool"),

    lc_string_comptime("int8"),
    lc_string_comptime("int16"),
    lc_string_comptime("int32"),
    lc_string_comptime("int64"),
    lc_string_comptime("uint8"),
    lc_string_comptime("uint16"),
    lc_string_comptime("uint32"),
    lc_string_comptime("uint64"),
    lc_string_comptime("usize"),

    lc_string_comptime("float32"),
    lc_string_comptime("float64"),
};

lc_bool lc_type_is_int(lc_type type)
{
    switch (type)
    {
    case LCT_INT8:
    case LCT_INT16:
    case LCT_INT32:
    case LCT_INT64:
    case LCT_UINT8:
    case LCT_UINT16:
    case LCT_UINT32:
    case LCT_UINT64:
    case LCT_USIZE:
        return true;

    default:
        return false;
    }
}

lc_bool lc_type_is_float(lc_type type)
{
    switch (type)
    {
    case LCT_FLOAT32:
    case LCT_FLOAT64:
        return true;

    default:
        return false;
    }
}

const lc_string *lc_type_name(lc_type type)
{
    return &STATIC_TYPE_NAMES[type];
}

lc_string *lc_string_new(const lc_char *str, lc_usize size)
{
    lc_string *str_ptr;
    lc_error_return_if(LC_E_ALLOC_FAILED, false, !(str_ptr = lc_mem_alloc(sizeof(lc_string))));

    *str_ptr = (lc_string){
        .data = lc_mem_alloc(size + 1),
        .size = size,
    };

    if (!str_ptr->data)
    {
        lc_error_set(LC_E_ALLOC_FAILED, "str_ptr->data");
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
    int length;
    lc_error_return_if(LC_E_OTHER, false, (length = vsnprintf(NULL, 0, format, args)) < 0);
    va_end(args);

    // allocate string of said buffer and call vsnprintf
    lc_string *str;
    lc_error_return_if(LC_E_ALLOC_FAILED, false, !(str = lc_string_new(NULL, length)));

    va_start(args, format);
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
    if (!str)
        return;

    lc_mem_free(str->data);
    lc_mem_free(str);
}

lc_list *lc_list_new_sized(lc_usize el_size, lc_usize capacity)
{
    lc_list *list;
    lc_error_return_if(LC_E_ALLOC_FAILED, false, !(list = lc_mem_alloc(sizeof(lc_list))));

    *list = (lc_list){
        .data = lc_mem_alloc(el_size * capacity),
        .capt = capacity,
        .el_size = el_size,
        .size = 0,
    };

    if (!list->data)
    {
        lc_mem_free(list);
        lc_error_set(LC_E_ALLOC_FAILED, "list->ptr");
        return NULL;
    }

    return list;
}

lc_bool lc_list_append(lc_list *list, void *value)
{
    lc_error_return_if(LC_E_INVALID_ARGUMENT, false, !list);

    if (list->size >= list->capt)
    {
        lc_usize new_capt = list->capt * 2;
        lc_void *new_ptr = lc_mem_realloc(list->data, list->el_size * new_capt);

        lc_error_return_if(LC_E_ALLOC_FAILED, false, !new_ptr);
        list->data = new_ptr;
        list->capt = new_capt;
    }

    lc_mem_copy(list->data + (list->size++) * list->el_size, value, list->el_size);
    return true;
}

lc_bool lc_list_remove(lc_list *list, lc_usize index)
{
    lc_error_return_if(LC_E_INVALID_ARGUMENT, false, index >= list->size);
    if (index + 1 < list->size)
        lc_mem_copy(list->data + index * list->el_size, list->data + (index + 1) * list->el_size, (list->size - index - 1) * list->el_size);

    list->size--;
    return true;
}

lc_void lc_list_free(lc_list *list)
{
    lc_mem_free(list->data);
    lc_mem_free(list);
}

lc_void lc_list_dump(const lc_list *list)
{
    printf("lc_list { size: %lu, capt: %lu }\n", list->size, list->capt);
}

lc_value lc_value_new(lc_type type, lc_usize data)
{
    return (lc_value){
        .type = type,
        .data = data,
    };
}

lc_void lc_value_dump(const lc_value *value)
{
    printf("lc_value { type: %s, data: %lu }\n", lc_type_name(value->type)->data, value->data);
}

lc_void lc_value_free(lc_value *value)
{
    switch (value->type)
    {
    case LCT_STRING:
        lc_string_free((lc_string *)value->data);
        break;

    case LCT_LIST:
    case LCT_STRUCT:
        lc_mem_free((void *)value->data);
        break;

    default:
        break;
    }
}
