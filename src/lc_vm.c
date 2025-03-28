#include "lc_vm.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// internal functions
static inline lc_bool lc_vm_type_new(lc_vm *vm, lc_type type);

lc_vm *lc_vm_new(const lc_uint8 *bytecode, lc_usize bytecode_size)
{
    lc_vm *vm;
    lc_error_return_if(LCE_ALLOC_FAILED, NULL, !(vm = lc_mem_alloc(sizeof(lc_vm))));

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
    for (lc_int32 i = 0; i < vm->top; i++)
        lc_value_free(&vm->stack[i]);

    lc_mem_free(vm);
}

lc_void lc_vm_dump(lc_vm *vm)
{
    printf("lc_vm (%p) stack: \n", vm);
    for (lc_int32 i = 0; i < vm->top; i++)
    {
        printf("    %d: ", (vm->top - i) - 1);
        lc_value_dump(&vm->stack[i]);
    }
}

inline lc_bool lc_vm_push_value(lc_vm *vm, lc_value *value)
{
    if (vm->top >= (lc_int32)sizeof(vm->stack))
        return false;

    lc_mem_copy(&vm->stack[vm->top++], value, sizeof(lc_value));
    return true;
}

inline lc_void lc_vm_pop(lc_vm *vm, lc_int32 count)
{
    while (count-- > 0)
    {
        if (vm->top == 0)
            return;

        lc_value_free(&vm->stack[--vm->top]);
    }
}

inline lc_value *lc_vm_to_value(lc_vm *vm, lc_int32 index)
{
    const lc_int32 idx = vm->top - 1 - index;
    lc_error_return_if(LCE_INVALID_ARGUMENT, NULL, idx < 0 || idx >= vm->top);
    return &vm->stack[idx];
}

inline lc_string *lc_vm_to_string(lc_vm *vm, lc_int32 index)
{
    lc_value *value = lc_vm_to_value(vm, index);
    if (!value)
        return NULL;

    if (value->type != LCT_STRING)
        return NULL;

    return (lc_string *)value->data;
}

inline lc_void *lc_vm_to_int(lc_vm *vm, lc_int32 index)
{
    lc_value *value = lc_vm_to_value(vm, index);
    if (!value)
        return NULL;

    if (!lc_type_is_int(value->type))
        return NULL;

    return (lc_void *)&value->data;
}

inline lc_void *lc_vm_to_float(lc_vm *vm, lc_int32 index)
{
    lc_value *value = lc_vm_to_value(vm, index);
    if (!value)
        return NULL;

    if (!lc_type_is_float(value->type))
        return NULL;

    return (lc_void *)&value->data;
}

inline lc_int32 lc_vm_top(lc_vm *vm)
{
    return vm->top;
}

lc_int32 lc_vm_run(lc_vm *vm)
{
    vm->btc_roller = 0;
    vm->top = 0;

    while (vm->btc_roller < vm->btc_size)
    {
        switch ((lc_vm_op)vm->btc[vm->btc_roller++])
        {
        case LCOP_NOOP:
            break;

        case LCOP_PUSH: {
            lc_uint8 type;
            lc_error_return_if(LCE_VM_EARLY_EOF, -1, vm->btc_roller + sizeof(type) > vm->btc_size);
            lc_mem_copy(&type, vm->btc + vm->btc_roller, sizeof(type));
            vm->btc_roller += sizeof(type);

            if (!lc_vm_type_new(vm, type))
                return -1;

            break;
        }

        case LCOP_IADD: {
            lc_value *a = lc_vm_to_value(vm, 1);
            lc_value *b = lc_vm_to_value(vm, 0);

            if (!a || !b)
                return -1;

            lc_value value = {
                a->type,
                a->data + b->data,
            };

            lc_vm_pop(vm, 2);
            if (!lc_vm_push_value(vm, &value))
                return -1;
            break;
        }

        case LCOP_ISUB: {
            lc_value *a = lc_vm_to_value(vm, 1);
            lc_value *b = lc_vm_to_value(vm, 0);

            if (!a || !b)
                return -1;

            lc_value value = {
                a->type,
                a->data - b->data,
            };

            lc_vm_pop(vm, 2);
            if (!lc_vm_push_value(vm, &value))
                return -1;
            break;
        }

        case LCOP_IMUL: {
            lc_value *a = lc_vm_to_value(vm, 1);
            lc_value *b = lc_vm_to_value(vm, 0);

            if (!a || !b)
                return -1;

            lc_value value = {
                a->type,
                a->data * b->data,
            };

            lc_vm_pop(vm, 2);
            if (!lc_vm_push_value(vm, &value))
                return -1;
            break;
        }

        case LCOP_IDIV: {
            lc_value *a = lc_vm_to_value(vm, 1);
            lc_value *b = lc_vm_to_value(vm, 0);

            if (!a || !b)
                return -1;

            lc_value value = {
                a->type,
                a->data / b->data,
            };

            lc_vm_pop(vm, 2);
            if (!lc_vm_push_value(vm, &value))
                return -1;
            break;
        }

        case LCOP_POP:
            break;

        case LCOP_CALL:
            break;

        default:
            return -1;
        }
    }

    return lc_vm_top(vm);
}

static inline lc_bool lc_vm_type_new(lc_vm *vm, lc_type type)
{
    switch (type)
    {
    case LCT_STRUCT:
        return -1;

    case LCT_VOID: {
        lc_value value = lc_value_new(LCT_VOID, 0);
        lc_vm_push_value(vm, &value);
        break;
    }

    case LCT_NULL: {
        lc_value value = lc_value_new(LCT_NULL, 0);
        lc_vm_push_value(vm, &value);
        break;
    }

    case LCT_STRING: {
        lc_uint64 len;
        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(len) > vm->btc_size);
        lc_mem_copy(&len, vm->btc + vm->btc_roller, sizeof(len));
        vm->btc_roller += sizeof(len);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + len > vm->btc_size);
        lc_string *text = lc_string_new((const lc_char *)(vm->btc + vm->btc_roller), len);
        vm->btc_roller += len;

        lc_value value = lc_value_new(LCT_STRING, (lc_usize)text);
        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_LIST: {
        break;
    }

    case LCT_BOOL: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_int8) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_int8));
        vm->btc_roller += sizeof(lc_int8);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_INT8:
    case LCT_UINT8: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_int8) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_int8));
        vm->btc_roller += sizeof(lc_int8);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_INT16:
    case LCT_UINT16: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_int16) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_int16));
        vm->btc_roller += sizeof(lc_int16);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_INT32:
    case LCT_UINT32: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_int32) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_int32));
        vm->btc_roller += sizeof(lc_int32);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_INT64:
    case LCT_UINT64:
    case LCT_USIZE: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_int64) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_int64));
        vm->btc_roller += sizeof(lc_int64);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_FLOAT32: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_float32) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_float32));
        vm->btc_roller += sizeof(lc_float32);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }

    case LCT_FLOAT64: {
        lc_value value = lc_value_new(type, 0);

        lc_error_return_if(LCE_VM_EARLY_EOF, false, vm->btc_roller + sizeof(lc_float64) > vm->btc_size);
        lc_mem_copy(&value.data, vm->btc + vm->btc_roller, sizeof(lc_float64));
        vm->btc_roller += sizeof(lc_float64);

        lc_error_return_if(LCE_VM_STACK_OVERFLOW, false, !lc_vm_push_value(vm, &value));
        break;
    }
    }

    return true;
}
