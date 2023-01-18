#include "coroutine.h"
#include <stdio.h>

void test(scheduler *sch, void *args) {
    char *str = (char *) args;
    printf("co start, args: %s, co id: %d\n", str, sch->running_co->id);
    printf("co before yield, co id: %d\n", sch->running_co->id);
    yield_coroutine(sch);
    printf("co resume after yield, co id: %d\n", sch->running_co->id);
    printf("co end, co id: %d\n", sch->running_co->id);
}

int main() {
    scheduler *sch = new_scheduler(1);

    char arg1[] = "test1";
    char arg2[] = "test2";

    int co_id1 = new_coroutine(sch, test, (void *) arg1);
    int co_id2 = new_coroutine(sch, test, (void *) arg2);

    printf("==== resume start ====\n");
    resume_coroutine(sch, co_id1);
    resume_coroutine(sch, co_id2);
    printf("=====\n");
    resume_coroutine(sch, co_id2);
    resume_coroutine(sch, co_id1);

    printf("==== resume end ====\n");

    close_scheduler(sch);
    return 0;
}
