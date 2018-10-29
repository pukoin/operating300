#include "klock.h"
#include <stdlib.h>
#include <stdio.h>

static nodeStruct *Graph = NULL;
static pthread_mutex_t RAGmutex = PTHREAD_MUTEX_INITIALIZER;

void init_lock(SmartLock* lock) {
  static long autoId = 0;
  autoId++;

  lock->id = autoId;

	pthread_mutex_init(&(lock->mutex), NULL);
}

int lock(SmartLock* lock) {
  pthread_mutex_lock(&RAGmutex);

  nodeStruct *threadNode = List_findNode(Graph, pthread_self());
  nodeStruct *lockNode = List_findNode(Graph, lock->id);

  // Does the graph have this thread already ?
  if (threadNode == NULL) {
    threadNode = List_createNode(pthread_self());
    List_insertTail(&(Graph), threadNode);
  }

  // Does the graph have this lock already ?
  if (lockNode == NULL) {
    lockNode = List_createNode(lock->id);
    List_insertTail(&(Graph), lockNode);
  }

  // Update the graph with boths nodes, thread and lock.
  // And an edge joining them.
  List_insertInComing(&(lockNode->incomingList), threadNode);
  List_insertOutGoing(&(threadNode->outgoingList), lockNode);

  // We first check if there is a cycle after adding these two
  // new edges. If there is one, we delete the newly added
  // edges and reject the request for a lock.
  // Otherwise, we check if this lock is available,
  // i.e, if there is an edge LOCK ---> SOME THREAD,
  // if there is one, we don't need to do anything
  // since we have already added the request edges.
  // If there is not one edge of that form, we can
  // grant this lock to the thread. We change from a request
  // edge to a granted edge.
  if (List_hasCycle(Graph)) {
    List_deleteInComing(&(lockNode->incomingList), threadNode);
    List_deleteOutGoing(&(threadNode->outgoingList), lockNode);
    pthread_mutex_unlock(&RAGmutex);
    return 0;
  } else {
    // We release this graph lock and wait for the smart lock
    pthread_mutex_unlock(&RAGmutex);

    // When the smart lock is done, we can then update the graph
    // with the new granted resource edges
    pthread_mutex_lock(&(lock->mutex));

    // Update the graph with the granted edges.
    pthread_mutex_lock(&RAGmutex);
    if (lockNode->outgoingList == NULL) {
      List_deleteInComing(&(lockNode->incomingList), threadNode);
      List_deleteOutGoing(&(threadNode->outgoingList), lockNode);

      List_insertOutGoing(&(lockNode->outgoingList), threadNode);
      List_insertInComing(&(threadNode->incomingList), lockNode);
    }
    pthread_mutex_unlock(&RAGmutex);

    return 1;
  }
}

void unlock(SmartLock* lock) {
  pthread_mutex_lock(&RAGmutex);

  nodeStruct *threadNode = List_findNode(Graph, pthread_self());
  nodeStruct *lockNode = List_findNode(Graph, lock->id);

  List_deleteInComing(&(threadNode->incomingList), lockNode);
  List_deleteOutGoing(&(lockNode->outgoingList), threadNode);

  pthread_mutex_unlock(&RAGmutex);
  pthread_mutex_unlock(&(lock->mutex));
}

/*
 * Cleanup any dynamic allocated memory for SmartLock to avoid memory leak
 * You can assume that cleanup will always be the last function call
 * in main function of the test cases.
 */
void cleanup() {
  nodeStruct *current = Graph;
  while(current != NULL) {
    cleanupEdgeList(current->incomingList);
    cleanupEdgeList(current->outgoingList);

    nodeStruct *tmp = current->next;
    free(current);
    current = tmp;
  }
}
