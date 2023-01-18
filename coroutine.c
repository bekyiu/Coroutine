//
// Created by bekyiu on 2023/1/18.
//

#include "coroutine.h"

void ensure(int ret, char msg[]) {
    if (!ret) {
        printf("%s\n", msg);
    }
    assert(ret);
}

void coroutine_wrapper(uint32_t low32, uint32_t hi32) {
    uintptr_t ptr = (uintptr_t) low32 | (((uintptr_t) hi32) << 32);
    scheduler *sch = (scheduler *) ptr;
    coroutine *co = sch->running_co;
    co->state = CO_RUNNING;
    co->body(sch, co->args);
    co->state = CO_DEAD;
    sch->running_co = NULL;
    close_coroutine(sch, co->id);
}

scheduler *new_scheduler(int cap) {
    ensure(cap > 0, "非法容量");

    scheduler *sch = (scheduler *) malloc(sizeof(scheduler));

    sch->list = (coroutine **) malloc(sizeof(coroutine *) * cap);
    memset(sch->list, 0, sizeof(coroutine *) * cap);
    sch->cap = cap;
    sch->size = 0;
    sch->running_co = NULL;

    return sch;
}

void expand_coroutine_list(scheduler *sch) {
    printf("触发扩容\n");
    int old_cap = sch->cap;
    coroutine **old_list = sch->list;

    int new_cap = old_cap * 2;
    coroutine **new_list = (coroutine **) malloc(sizeof(coroutine *) * new_cap);
    memset(new_list, 0, sizeof(coroutine *) * new_cap);

    for (int i = 0; i < old_cap; i++) {
        new_list[i] = old_list[i];
    }

    sch->list = new_list;
    sch->cap = new_cap;
    free(old_list);
}

int allocate_coroutine_id(scheduler *sch) {
    int co_id = -1;

    if (sch->size < sch->cap) {
        for (int i = 0; i < sch->cap; i++) {
            if (sch->list[i] == NULL) {
                co_id = i;
                break;
            }
        }

        ensure(co_id != -1, "sch->list未释放干净");
        return co_id;
    }

    // 扩容
    expand_coroutine_list(sch);
    return allocate_coroutine_id(sch);
}

int new_coroutine(scheduler *sch, co_body body, void *args) {

    int co_id = allocate_coroutine_id(sch);
    coroutine *co = (coroutine *) malloc(sizeof(coroutine));
    co->id = co_id;
    co->body = body;
    co->args = args;
    co->state = CO_READY;

    ucontext_t ctx;
    getcontext(&ctx);
    ctx.uc_stack.ss_sp = co->stack;
    ctx.uc_stack.ss_size = sizeof(co->stack);
    // 这个协程执行完了需要跳回调用处
    ctx.uc_link = &sch->main_ctx;
    // 把地址分成两个32bit传递 兼容mac os
    uintptr_t sch_addr = (uintptr_t) sch;
    makecontext(&ctx, (void (*)(void)) coroutine_wrapper, 2, (uint32_t) sch_addr, (uint32_t) (sch_addr >> 32));
    co->ctx = ctx;

    sch->list[co_id] = co;
    sch->size += 1;
    return co_id;
}

void resume_coroutine(scheduler *sch, int co_id) {
    coroutine *co = sch->list[co_id];
    ensure(co->state == CO_READY || co->state == CO_SUSPEND, "启动协程状态异常");
    sch->running_co = co;
    swapcontext(&(sch->main_ctx), &(co->ctx));
}

void yield_coroutine(scheduler *sch) {
    coroutine *co = sch->running_co;
    co->state = CO_SUSPEND;
    sch->running_co = NULL;
    swapcontext(&(co->ctx), &(sch->main_ctx));
}

void close_coroutine(scheduler *sch, int co_id) {
    coroutine *co = sch->list[co_id];
    if (co == NULL) {
        return;
    }


    if (co == sch->running_co) {
        sch->running_co = NULL;
    }

    free(co);
    sch->list[co_id] = NULL;
    sch->size -= 1;
}

void close_scheduler(scheduler *sch) {
    for (int i = 0; i < sch->cap; i++) {
        close_coroutine(sch, i);
    }
    free(sch);
}