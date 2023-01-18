# A simple coroutine implementation written in C

### run
```shell
make
./app
```

### api
```c
// create a coroutine scheduler
// cap: how many coroutines the scheduler can hold
// return: the new scheduler
scheduler *new_scheduler(int cap);

// create a coroutine
// sch: the coroutine scheduler
// co_body: it's a typedef 
//      typedef void (*co_body)(scheduler *sch, void *args);
//      represent coroutine body
// args: the co_body's parameters
// return: coroutine id
int new_coroutine(scheduler *sch, co_body body, void *args);

// resume coroutine
// sch: the scheduler
// co_id: coroutine id
void resume_coroutine(scheduler *sch, int co_id);

// yield current running coroutine
// sch: the coroutine scheduler
void yield_coroutine(scheduler *sch);

// free the coroutine
// sch: the scheduler
void close_coroutine(scheduler *sch, int co_id);

// free the scheduler
// sch: the scheduler
void close_scheduler(scheduler *sch);
```

### example
```c
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
```

result:
```shell
==== resume start ====
co start, args: test1, co id: 0
co before yield, co id: 0
co start, args: test2, co id: 1
co before yield, co id: 1
=====
co resume after yield, co id: 1
co end, co id: 1
co resume after yield, co id: 0
co end, co id: 0
==== resume end ====
```