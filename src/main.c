#include "lc.h"
#include "lc_types.h"
#include "lc_vm.h"

int main()
{
    lc_vm *vm = lc_vm_new();
    lc_value *value = lc_value_new(&STATIC_TYPES[0], NULL, 0, false);

    lc_vm_push(vm, value);
    lc_vm_dump(vm);
    lc_vm_pop(vm);

    lc_vm_free(vm);
    return 0;
}