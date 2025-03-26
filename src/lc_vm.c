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

lc_vm *lc_vm_new(const lc_uint8 *bytecode, lc_usize bytecode_size)
{
    lc_vm *vm = lc_mem_alloc(sizeof(lc_vm));
    if (!vm)
    {
        lc_error_set(LCE_ALLOC_FAILED, "vm");
        return NULL;
    }

    *vm = (lc_vm){
        .types = lc_list_new(lc_type *, 256),
        .stack = {},
        .top = 0,

        .btc = lc_mem_dupe(bytecode, bytecode_size),
        .btc_size = bytecode_size,
        .btc_roller = 0,
    };

    if (!vm->btc)
    {
        lc_error_set(LCE_ALLOC_FAILED, "vm->bytecode");
        lc_mem_free(vm);
        return NULL;
    }

    if (!vm->types)
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
    return vm->stack[vm->top - 1];
}

lc_void lc_vm_pop(lc_vm *vm)
{
    if (vm->top == 0)
        return;

    lc_value_free(vm->stack[--vm->top]);
}

lc_int32 lc_vm_run(lc_vm *vm)
{
    vm->btc_roller = 0;
    while (vm->btc_roller < vm->btc_size)
    {
        switch ((lc_vm_op)vm->btc[vm->btc_roller++])
        {
        case LCOP_NOOP:
            break;

        case LCOP_PUSHNEW: {
            lc_uint64 idx;
            lc_mem_copy(&idx, vm->btc + vm->btc_roller, sizeof(idx));
            vm->btc_roller += sizeof(idx);

            switch (TYPE_TABLE[idx].kind)
            {
            case LCT_FLOAT:
            case LCT_STRUCT:
            case LCT_VOID:
            case LCT_LIST:
                return -1;

            case LCT_NULL: {
                lc_value *value = lc_value_new(&TYPE_TABLE[idx], NULL, 0, false);
                lc_vm_push(vm, value);
                break;
            }

            case LCT_BOOL: {
                lc_bool *ptr = lc_mem_alloc(sizeof(lc_bool));
                *ptr = vm->btc[vm->btc_roller++];

                lc_value *value = lc_value_new(&TYPE_TABLE[idx], ptr, sizeof(lc_bool), false);
                lc_vm_push(vm, value);
                break;
            }

            case LCT_STRING: {
                lc_uint64 len;
                lc_mem_copy(&len, vm->btc + vm->btc_roller, sizeof(len));
                vm->btc_roller += sizeof(len);

                lc_string *text = lc_string_new((const lc_char *)(vm->btc + vm->btc_roller), len);
                vm->btc_roller += len;

                lc_value *value = lc_value_new(&TYPE_TABLE[idx], text, sizeof(lc_string), false);
                lc_vm_push(vm, value);
                break;
            }

            case LCT_INTEGER: {
                const lc_type *type = &TYPE_TABLE[idx];
                const lc_number *number = type->data;

                lc_void *data = lc_mem_alloc(number->bits);
                lc_mem_copy(data, vm->btc + vm->btc_roller, number->bits);
                vm->btc_roller += number->bits;

                lc_value *value = lc_value_new(type, data, number->bits, false);
                lc_vm_push(vm, value);
                break;
            }
            }
            break;
        }

        case LCOP_PUSHVALUE:
            break;

        case LCOP_POP:
            break;

        case LCOP_CALL:
            break;

        default:
            return -1;
        }
    }
}