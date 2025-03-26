#ifndef LC_TYPES_H
#define LC_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void lc_void;

/* Numeric types */

typedef int8_t lc_int8;
typedef int16_t lc_int16;
typedef int32_t lc_int32;
typedef int64_t lc_int64;

typedef uint8_t lc_uint8;
typedef uint16_t lc_uint16;
typedef uint32_t lc_uint32;
typedef uint64_t lc_uint64;

typedef lc_uint64 lc_usize;
typedef bool lc_bool;
typedef char lc_char;

typedef float lc_float32;
typedef double lc_float64;

typedef enum
{
    LCT_VOID,
    LCT_NULL,
    LCT_BOOL,
    LCT_STRING,

    LCT_INTEGER,
    LCT_FLOAT,
    LCT_LIST,
    LCT_STRUCT
} lc_type_kind;

typedef struct
{
    lc_uint32 size;
    lc_bool sign;
} lc_type_number;

typedef struct
{
    lc_char *data;
    lc_usize size;
} lc_string;

typedef struct
{
    lc_void *ptr;
    lc_usize capt, el_size, size;
} lc_list;

typedef struct
{

} lc_struct;

typedef struct
{
    lc_type_kind kind;
    lc_string idnt;
    lc_void *data;
} lc_type;

typedef struct
{
    const lc_type *type;

    lc_usize data;
    lc_bool ptr;
} lc_value;

#define lc_string_comptime(str) ((lc_string){str, (sizeof(str) - 1)})
lc_string *lc_string_new(const lc_char *str, lc_usize size);
lc_void lc_string_free(lc_string *str);
lc_string *lc_string_format(const lc_char *format, ...);
lc_string *lc_string_dupe(const lc_string *str);

#define lc_list_foreach(list, name) for (lc_void *name = list->ptr; name < list->ptr + list->size * list->el_size; name += list->el_size)
#define lc_list_new(type, capacity) lc_list_new_sized(sizeof(type), capacity)
lc_list *lc_list_new_sized(lc_usize el_size, lc_usize capacity);
lc_bool lc_list_append(lc_list *list, void *value);
lc_bool lc_list_remove(lc_list *list, lc_usize index);
lc_void lc_list_free(lc_list *list);
lc_void lc_list_dump(const lc_list *list);

lc_value lc_value_new(const lc_type *type, lc_usize data, lc_bool ptr);
lc_void lc_value_free(lc_value *value);
lc_void lc_value_dump(const lc_value *value);

#endif
