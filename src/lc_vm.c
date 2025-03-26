#include "lc_vm.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include <stddef.h>
#include <stdio.h>

static lc_number TYPE_NUMERIC_DATA[] = {
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

static lc_type TYPE_TABLE[] = {
    {LCT_VOID, lc_string_comptime("void"), NULL},
    {LCT_NULL, lc_string_comptime("null"), NULL},
    {LCT_BOOL, lc_string_comptime("bool"), NULL},
    {LCT_STRING, lc_string_comptime("string"), NULL},
    {LCT_LIST, lc_string_comptime("list"), NULL},

    {LCT_INTEGER, lc_string_comptime("int8"), &TYPE_NUMERIC_DATA[0]},
    {LCT_INTEGER, lc_string_comptime("int16"), &TYPE_NUMERIC_DATA[1]},
    {LCT_INTEGER, lc_string_comptime("int32"), &TYPE_NUMERIC_DATA[2]},
    {LCT_INTEGER, lc_string_comptime("int64"), &TYPE_NUMERIC_DATA[3]},
    {LCT_INTEGER, lc_string_comptime("uint8"), &TYPE_NUMERIC_DATA[4]},
    {LCT_INTEGER, lc_string_comptime("uint16"), &TYPE_NUMERIC_DATA[5]},
    {LCT_INTEGER, lc_string_comptime("uint32"), &TYPE_NUMERIC_DATA[6]},
    {LCT_INTEGER, lc_string_comptime("uint64"), &TYPE_NUMERIC_DATA[7]},
    {LCT_INTEGER, lc_string_comptime("usize"), &TYPE_NUMERIC_DATA[8]},

    {LCT_FLOAT, lc_string_comptime("float32"), &TYPE_NUMERIC_DATA[9]},
    {LCT_FLOAT, lc_string_comptime("float64"), &TYPE_NUMERIC_DATA[10]},
};

lc_vm *lc_vm_new(void)
{
    lc_vm *vm = lc_mem_alloc(sizeof(lc_vm));
    if (!vm)
    {
        lc_error_set(LCE_ALLOC_FAILED, "vm");
        return NULL;
    }

    *vm = (lc_vm){
        .stack = {},
        .types = lc_list_new(lc_type*, 256),
        .top = 0,
    };

    if(!vm->types)
    {
        lc_error_set(LCE_ALLOC_FAILED, "vm->types");
        lc_mem_free(vm);
        return NULL;
    }

    return vm;
}

lc_void lc_vm_free(lc_vm *vm)
{
    for (lc_usize i = 0; i < vm->top; i++)
        lc_value_free(vm->stack[i]);

    lc_mem_free(vm);
}

lc_void lc_vm_dump(lc_vm *vm)
{
    printf("lc_vm (%p) stack: \n", vm);
    for (lc_usize i = 0; i < vm->top; i++)
    {
        printf("    %lu: ", i);
        lc_value_dump(vm->stack[i]);
    }
}

lc_void lc_vm_push(lc_vm *vm, lc_value *value)
{
    vm->stack[vm->top++] = value;
}

lc_usize lc_vm_top(lc_vm *vm)
{
    return vm->top;
}

lc_value *lc_vm_value(lc_vm *vm)
{
    return vm->stack[vm->top];
}

lc_void lc_vm_pop(lc_vm *vm)
{
    if (vm->top == 0)
        return;

    lc_value_free(vm->stack[--vm->top]);
}
