#include "lc_types.h"
#include "lc_error.h"
#include "lc_mem.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

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

lc_list *lc_list_new_sized(lc_usize el_size, lc_usize capacity)
{
    lc_list *list = lc_mem_alloc(sizeof(lc_list));
    if (!list)
    {
        lc_error_set(LCE_ALLOC_FAILED, "list");
        return NULL;
    }

    *list = (lc_list){
        .ptr = lc_mem_alloc(el_size * capacity),
        .capt = capacity,
        .el_size = el_size,
        .size = 0,
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
        lc_void *new_ptr = lc_mem_realloc(list->ptr, list->el_size * new_capt);

        if (!new_ptr)
        {
            lc_error_set(LCE_ALLOC_FAILED, "new_ptr");
            return false;
        }

        list->ptr = new_ptr;
        list->capt = new_capt;
    }

    lc_mem_copy(list->ptr + (list->size++) * list->el_size, value, list->el_size);
    return true;
}

lc_bool lc_list_remove(lc_list *list, lc_usize index)
{
    if (index >= list->size)
    {
        lc_error_set(LCE_INVALID_ARGUMENT, "index");
        return false;
    }

    if (index + 1 < list->size)
        lc_mem_copy(list->ptr + index * list->el_size, list->ptr + (index + 1) * list->el_size, (list->size - index - 1) * list->el_size);

    list->size--;
    return true;
}

lc_void lc_list_free(lc_list *list)
{
    lc_mem_free(list->ptr);
    lc_mem_free(list);
}

lc_void lc_list_dump(const lc_list *list)
{
    printf("lc_list { size: %lu, capt: %lu }\n", list->size, list->capt);
}

lc_value *lc_value_new(const lc_type *type, lc_void *data, lc_usize size, bool ptr)
{
    lc_value *value = lc_mem_alloc(sizeof(lc_value));
    if (!value)
    {
        lc_error_set(LCE_ALLOC_FAILED, "value");
        return NULL;
    }

    *value = (lc_value){
        .type = type,
        .data = data,
        .size = size,
        .ptr = ptr,
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
