/*
 * types used by thread library
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

typedef struct tcb_t { //Thread Control block
    int         thread_id;
    int         thread_priority;
    ucontext_t *thread_context;
    struct tcb_t *next;
} tcb_t;

typedef struct tQueue_t{ //Thread Queue which consists of TCB
  tcb_t *head, *tail; //Use a head and tail tcb for ease of added to the end
} tQueue_t;

typedef struct {
        int count;
        tQueue_t *q;
    } sem_t;

tcb_t *allocTCB(ucontext_t *context, int thread_id, int thread_p);
tQueue_t *allocQueue();
void addTCB(tQueue_t *queue, tcb_t *tcb);
tcb_t *removeTCB(tQueue_t *queue, int thread_id);