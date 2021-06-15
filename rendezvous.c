#include <stdio.h>
#include <unistd.h>
#include "ud_thread.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

sem_t *s1, *s2;

void child_1(int arg) {
    printf("child 1: before\n");
    // what goes here?
    sem_wait(s2);
    printf("child 1: after\n");
    sem_signal(s1);
    t_terminate();
}

void child_2(int arg) {
    printf("child 2: before\n");
    // what goes here?
    sem_signal(s2);
    printf("child 2: after\n");
    sem_signal(s1);
    t_terminate();
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    // init semaphores here
    t_init();
    sem_init(&s1, 0);
    sem_init(&s2, 0);
    t_create(child_1, 1,1);
    t_create(child_2, 2,1);
    sem_wait(s1);
    sem_wait(s1);
    t_shutdown();
    printf("parent: end\n");
    return 0;
}
