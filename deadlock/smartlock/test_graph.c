#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "klock.h"
#include <assert.h>

/*
 * This is a simple deadlock condition similar to the RAG showed in
 * assignment web page.
 * The critical sections of thread_0 and thread_1 should both be able
 * to invoke if the SmartLock is implemented correctly, and there should
 * be no memory leak after finishing the main function.
 */
int main(int argc, char *argv[]) {
    nodeStruct *graph = NULL;
    nodeStruct *n1 = List_createNode(1);
    nodeStruct *n2 = List_createNode(2);
    nodeStruct *n3 = List_createNode(3);
    nodeStruct *n4 = List_createNode(4);
    nodeStruct *n5 = List_createNode(5);
    nodeStruct *n6 = List_createNode(6);

    List_insertTail(&graph, n1);
    List_insertTail(&graph, n2);
    List_insertTail(&graph, n3);
    List_insertTail(&graph, n4);
    List_insertTail(&graph, n5);
    List_insertTail(&graph, n6);

    List_insertInComing(&(n1->incomingList), n2);
    List_insertOutGoing(&(n2->outgoingList), n1);

    List_insertInComing(&(n3->incomingList), n2);
    List_insertOutGoing(&(n2->outgoingList), n3);

    List_insertInComing(&(n4->incomingList), n3);
    List_insertOutGoing(&(n3->outgoingList), n4);

    List_insertInComing(&(n5->incomingList), n4);
    List_insertOutGoing(&(n4->outgoingList), n5);

    List_insertInComing(&(n6->incomingList), n5);
    List_insertOutGoing(&(n5->outgoingList), n6);


    List_insertInComing(&(n1->incomingList), n6);
    List_insertOutGoing(&(n6->outgoingList), n1);

    List_insertInComing(&(n4->incomingList), n6);
    List_insertOutGoing(&(n6->outgoingList), n4);

    assert(List_hasCycle(graph));
    printf("PASSED: Test cycle detection: Case YES !!\n");

    List_deleteInComing(&(n4->incomingList), n6);
    List_deleteOutGoing(&(n6->outgoingList), n4);

    assert(!List_hasCycle(graph));
    printf("PASSED: Test cycle detection: Case NO !!\n");

    cleanup();
    return 0;
}
