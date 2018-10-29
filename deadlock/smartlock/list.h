// Linked list module.

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>

typedef struct nodeStruct {
  long id;
  struct nodeStruct *next;
  _Bool deleted;

  struct Edge *incomingList;
  struct Edge *outgoingList;
} nodeStruct;

typedef struct Edge {
  _Bool deleted;
  struct nodeStruct *vertex;
  struct Edge *next;
} Edge;

void cleanupEdgeList(Edge *node);
Edge* List_createEdge(nodeStruct *vertex);
Edge* List_insertInComing (Edge **incomingList, nodeStruct *target);
Edge* List_insertOutGoing (Edge **outgoingList, nodeStruct *target);

Edge* List_findInComing(Edge *head, long _id);
Edge* List_findOutGoing(Edge *head, long _id);

_Bool List_hasCycle(nodeStruct *graph);

void List_deleteOutGoing(Edge **incomingList, nodeStruct *node);
void List_deleteInComing (Edge **outgoingList, nodeStruct *node);

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(long _id);

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Insert node after the tail of the list.
 */
nodeStruct* List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node);


/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head);

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, long _id);
/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node);


#endif
