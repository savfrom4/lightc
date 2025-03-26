#include "lc.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include "lc_vm.h"
#include <stdio.h>

int main()
{
    lc_list* list = lc_list_new(void**, 1);
    if(!list)
    {
        printf("%s", lc_error_msg());
        return 1;
    }

    void** value = lc_mem_alloc(sizeof(void**));
    *value = NULL;

    lc_list_append(list, value);
    lc_list_append(list, value);
    lc_list_append(list, value);
    lc_list_append(list, value);

    lc_list_remove(list, 2);

    lc_list_dump(list);
    lc_list_foreach(list, it) {
        void** test = it;
        printf("    %p\n",  *test);
    }

    lc_list_free(list);
    return 0;
}
