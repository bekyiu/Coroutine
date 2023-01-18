//
// Created by bekyiu on 2023/1/18.
//

#ifndef COROUTINE_COROUTINE_H
#define COROUTINE_COROUTINE_H


#if __APPLE__ && __MACH__
#define _XOPEN_SOURCE 600
#endif

#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


#define CO_DEAD 0
#define CO_READY 1
#define CO_RUNNING 2
#define CO_SUSPEND 3
#define STACK_SIZE 2048

typedef struct scheduler scheduler;
typedef struct coroutine coroutine;

typedef void (*co_body)(scheduler *sch, void *args);

typedef struct coroutine {
    // 协程id
    int id;
    // 协程体
    co_body body;
    // 协程参数
    void *args;
    // 当前协程上下文
    ucontext_t ctx;
    // 当前协程栈
    char stack[STACK_SIZE];
    // 当前写成状态
    int state;
} coroutine;

typedef struct scheduler {
    // 协程列表
    coroutine **list;
    // 列表容量
    int cap;
    // 列表元素个数
    int size;
    // 当前正在运行的协程
    coroutine *running_co;
    // 发生调度(调用 resume, yield)的上下文
    ucontext_t main_ctx;
} scheduler;

scheduler *new_scheduler(int cap);

int new_coroutine(scheduler *sch, co_body body, void *args);

void resume_coroutine(scheduler *sch, int co_id);

void yield_coroutine(scheduler *sch);

void close_coroutine(scheduler *sch, int co_id);

void close_scheduler(scheduler *sch);


#endif //COROUTINE_COROUTINE_H
