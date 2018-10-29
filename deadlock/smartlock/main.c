#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "klock.h"
#include <pthread.h>

SmartLock glocks[2];

void *thread_0(void *arg) {
    while (lock(&glocks[0]) == 0); // Force locking glocks[0]
    sleep(1);
    while (lock(&glocks[1]) == 0); // Force locking glocks[1]

    printf("thread 0 is working on critical section for 1 second\n");
    sleep(1);

    unlock(&glocks[0]);
    unlock(&glocks[1]);

    return NULL;
}

void *thread_1(void *arg) {
    while (1) {
        int lock1_res = lock(&glocks[1]);
        sleep(2);
        if (lock1_res) {
            int lock0_res = lock(&glocks[0]);
            if (lock0_res) {
                printf("thread 1 is working on critical section for 1 second\n");
                sleep(1);
                unlock(&glocks[1]);
                unlock(&glocks[0]);
                break;
            } else {
            	// If thread_1 is not able to lock glocks[0] now, it will also
            	// unlock glocks[1] and sleep for 1 second before retry so that
            	// thread_0 can acquire glocks[1]
                unlock(&glocks[1]);
                sleep(1);
            }
        }
    }
    return NULL;
}

/*
 * This is a simple deadlock condition similar to the RAG showed in
 * assignment web page.
 * The critical sections of thread_0 and thread_1 should both be able
 * to invoke if the SmartLock is implemented correctly, and there should
 * be no memory leak after finishing the main function.
 */
int main(int argc, char *argv[]) {
    init_lock(&glocks[0]);
    init_lock(&glocks[1]);

    pthread_t tids[2];

    pthread_create(&tids[0], NULL, thread_0, NULL);
    pthread_create(&tids[1], NULL, thread_1, NULL);
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    // nodeStruct *graph = NULL;
    // nodeStruct *n1 = List_createNode(1);
    // nodeStruct *n2 = List_createNode(2);
    // nodeStruct *n3 = List_createNode(3);
    // nodeStruct *n4 = List_createNode(4);
    // nodeStruct *n5 = List_createNode(5);
    // nodeStruct *n6 = List_createNode(6);

    // List_insertTail(&graph, n1);
    // List_insertTail(&graph, n2);
    // List_insertTail(&graph, n3);
    // List_insertTail(&graph, n4);
    // List_insertTail(&graph, n5);
    // List_insertTail(&graph, n6);

    // printf("Goood\n");

    // List_insertInComing(&(n1->incomingList), n2);
    // List_insertOutGoing(&(n2->outgoingList), n1);
    // printf("Goood 2\n");

    // List_insertInComing(&(n3->incomingList), n2);
    // List_insertOutGoing(&(n2->outgoingList), n3);
    // printf("Goood 3\n");

    // List_insertInComing(&(n4->incomingList), n3);
    // List_insertOutGoing(&(n3->outgoingList), n4);
    // printf("Goood 4\n");

    // List_insertInComing(&(n5->incomingList), n4);
    // List_insertOutGoing(&(n4->outgoingList), n5);
    // printf("Goood 5\n");

    // List_insertInComing(&(n6->incomingList), n5);
    // List_insertOutGoing(&(n5->outgoingList), n6);
    // printf("Goood 6\n");

    // // List_insertInComing(&(n4->incomingList), n6);
    // // printf("Goood 6 partial\n");
    // // List_insertOutGoing(&(n6->outgoingList), n4);

    // List_insertInComing(&(n1->incomingList), n6);
    // printf("Goood 6 partial\n");
    // List_insertOutGoing(&(n6->outgoingList), n1);

    // // List_insertInComing(&(n4->incomingList), n1);
    // // printf("Goood 6 partial\n");
    // // List_insertOutGoing(&(n1->outgoingList), n4);

    // List_insertInComing(&(n4->incomingList), n6);
    // printf("Goood 6 partial\n");
    // List_insertOutGoing(&(n6->outgoingList), n4);

    // if (List_hasCycle(graph)) {
    //     printf("YES!!! has a cycle\n");
    // } else {

    //     printf("NOOO!!! DOES NOT HAVE a cycle\n");
    // }

    // nodeStruct *cursor = graph;
    // while (cursor != NULL) {
    //     deallocate(cursor->incomingList);
    //     deallocate(cursor->outgoingList);
    //     cursor = cursor->next;
    // }

    // free(n1);
    // free(n2);
    // free(n3);
    // free(n4);
    // free(n5);
    // free(n6);

    cleanup();
    return 0;
}
