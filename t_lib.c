#include "t_lib.h"

tQueue_t *running;
tQueue_t *ready;

void t_init()
{
  running = allocQueue();
  ready = allocQueue();
  ucontext_t *tmp = (ucontext_t *) malloc(sizeof(ucontext_t));
  if(getcontext(tmp) == -1){ /* let tmp be the context of main() */
    perror("getcontext");
    exit(EXIT_FAILURE);
  }    
  tcb_t *mainTCB = allocTCB(tmp,0,1);
  addTCB(running,mainTCB);
}

int t_create(void (*fct)(int), int id, int pri)
{
  size_t sz = 0x10000;

  ucontext_t *uc;
  uc = (ucontext_t *) malloc(sizeof(ucontext_t));

  getcontext(uc);
/***
  uc->uc_stack.ss_sp = mmap(0, sz,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_PRIVATE | MAP_ANON, -1, 0);
***/
  uc->uc_stack.ss_sp = malloc(sz);  /* new statement */
  uc->uc_stack.ss_size = sz;
  uc->uc_stack.ss_flags = 0;
  uc->uc_link = running->head->thread_context; 

  tcb_t *newTCB = allocTCB(uc,id,pri);

  makecontext(uc, (void (*)(void)) fct, 1, id);
  addTCB(ready,newTCB);
}

void t_yield()
{
  if(running != NULL && ready != NULL && ready->head != NULL){
    tcb_t *tmp = removeTCB(running,-1);
    addTCB(running,removeTCB(ready,-1));
    addTCB(ready,tmp);

    swapcontext(tmp->thread_context, running->head->thread_context);
  }
}

void t_terminate()
{
  tcb_t *tmp = removeTCB(running,-1);
  addTCB(running,removeTCB(ready,-1));
  free(tmp->thread_context->uc_stack.ss_sp);
  free(tmp->thread_context);
  free(tmp);
  setcontext(running->head->thread_context);
}

void t_shutdown()
{
  tcb_t *iter = running->head;
  while(iter != NULL){
    tcb_t *freeTCB = iter;
    iter = iter->next;
    free(freeTCB->thread_context->uc_stack.ss_sp);
    free(freeTCB->thread_context);
    free(freeTCB);
  }
  iter = ready->head;
  while(iter != NULL){
    tcb_t *freeTCB = iter;
    iter = iter->next;
    free(freeTCB->thread_context->uc_stack.ss_sp);
    free(freeTCB->thread_context);
    free(freeTCB);
  }
  if(ready != NULL)
    free(ready);
  if(running != NULL)
    free(running);
  
  ready = NULL;
  running = NULL;
}

tcb_t *allocTCB(ucontext_t *context, int thread_id, int thread_p){
  tcb_t *ret = (tcb_t *) malloc(sizeof(tcb_t));
  ret->thread_context = context; //Make sure to malloc space for this before passing in 
  ret->thread_id = thread_id;
  ret->thread_priority = thread_p;
  ret->next = NULL;
  return ret;
}

tQueue_t *allocQueue(){
  tQueue_t *ret = (tQueue_t *) malloc(sizeof(tQueue_t));
  ret->head = ret->tail = NULL;
  return ret;
}

void addTCB(tQueue_t *queue, tcb_t *tcb){
  if(queue->tail == NULL){ //Queue is empty
    queue->head = queue->tail = tcb;
    queue->tail->next = NULL;
  }
  else{
    queue->tail->next = tcb;
    queue->tail = tcb;
    queue->tail->next = NULL;
  }
}

tcb_t *removeTCB(tQueue_t *queue, int thread_id){
  if(queue->head == NULL){ //Empty queue
    return NULL;
  }
  else if((thread_id < 0) || (queue->head->thread_id == thread_id)){ //Thread IDs less than 0 signify the head of a queue
    tcb_t *ret = queue->head;
    queue->head = queue->head->next;
    if(queue->head == NULL)
      queue->tail = NULL;
    return ret;
  }
  else{ //Loop through queue until you find the thread id
    tcb_t *iter = queue->head;
    while(iter->next != NULL){
      if(iter->next->thread_id != thread_id){
        tcb_t *ret = iter->next;
        if(ret == queue->tail){
          queue->tail = iter;
          iter->next = NULL;
        }
        else{
          iter->next = ret->next;
        }
        return ret;
      }
      iter = iter->next;
    }
    return NULL;
  }
}

void sem_init(sem_t **sp, unsigned int count){
  *sp = malloc(sizeof(sem_t));
  (*sp)->count = count;
  (*sp)->q = allocQueue();
}

void sem_wait(sem_t *sp){
  sp->count--;
  if(sp->count < 0){
    if(running != NULL && ready != NULL && ready->head != NULL){
      tcb_t *tmp = removeTCB(running,-1);
      addTCB(sp->q,tmp);
      addTCB(running,removeTCB(ready,-1));
      swapcontext(tmp->thread_context, running->head->thread_context);
    }
  }
}

void sem_signal(sem_t *sp){
  sp->count++;
  if(sp->count <= 0){
    if(running != NULL && ready != NULL){
      tcb_t *tmp = removeTCB(sp->q,-1);
      addTCB(ready,tmp);
    }
  }
}

void sem_destroy(sem_t **sp){
  tcb_t *iter = (*sp)->q->head;
  while(iter != NULL){
    tcb_t *destroy = iter;
    iter = iter->next;
    addTCB(ready, destroy);
  }
  free((*sp)->q);
  free(*sp);
}