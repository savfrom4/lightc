#include "lc_vm.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include <stdio.h>

lc_vm *lc_vm_new(void)
{
    lc_vm *vm = lc_mem_alloc(sizeof(lc_vm));
    if (!vm)
    {
        lc_error_set(LCE_ALLOC_FAILED, "vm");
        return NULL;
    }

    *vm = (lc_vm){
        {},
        0,
    };

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