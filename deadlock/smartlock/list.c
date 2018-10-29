// This is a modified version of the list implementation
// from the solution of project 1.
// It has been fairly modified so that it supports
// a graph like structure.
// It represents a graph as an adjacency list using
// a linked list of nodes and two linked list for each
// node for incoming edges and outgoing edges.

#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void resetGraphState(nodeStruct *graph) {
	nodeStruct *adjacencyList = graph;

  while(adjacencyList != NULL) {
    adjacencyList->deleted = false;
    Edge *inCursor = adjacencyList->incomingList;

    while(inCursor != NULL) {
      inCursor->deleted = false;
      inCursor = inCursor->next;
    }
    adjacencyList = adjacencyList->next;
  }
}

static int toCount(Edge *node) {
  Edge *tmp = node;
  int count = 0;

  while(tmp!=NULL) {
    count++;
    tmp = tmp->next;
  }

  return count;
}

_Bool List_hasCycle(nodeStruct *graph) {
  if (graph == NULL) false;

  int nodesCount = 0;
  _Bool isConnected = true;

  Edge *noInDegNodes = NULL;
  Edge *topologicalOrder = NULL;
  nodeStruct *adjacencyList = graph;

  // Find nodes whose INDEGREE is zero, add them to the list of
  // noInDegNodes(no indegree nodes).
  while(adjacencyList != NULL) {
    if (adjacencyList->incomingList == NULL) {
      adjacencyList->deleted = true;
      List_insertInComing(&noInDegNodes, adjacencyList);
      isConnected = isConnected && (adjacencyList->outgoingList != NULL);
    }
    nodesCount++;
    adjacencyList = adjacencyList->next;
  }

  // Add the nodes with no indegree to the topologicalOrder list
  // And mark such node as being deleted. So that is not considered
  // in the next run.
  while(noInDegNodes != NULL) {
    adjacencyList = graph;
    // Add this node to the list that holds the topological order.
    List_insertInComing(&topologicalOrder, noInDegNodes->vertex);

    // Update all nodes that have "the node being deleted" as an incoming
    // neighbour. Now, they have one less incoming edge.
    while(adjacencyList != NULL) {
      Edge *inEdge = adjacencyList->incomingList;
      while(inEdge != NULL) {
        if (!inEdge->deleted && inEdge->vertex->id == noInDegNodes->vertex->id) {
          inEdge->deleted = true;
        }
        inEdge = inEdge->next;
      }
      adjacencyList = adjacencyList->next;
    }

    // After the update above, we check if there is any new
    // node that has no incoming edge. Excluding the deleted ones.
    adjacencyList = graph;
    while(adjacencyList != NULL) {
      if (adjacencyList->incomingList != NULL && !adjacencyList->deleted) {
        Edge *inCursor = adjacencyList->incomingList;
        int deletedCount = 0;
        int inDegree = 0;

        while (inCursor != NULL) {
          if (inCursor->deleted) deletedCount++;
          inDegree++;
          inCursor = inCursor->next;
        }

        if (inDegree == deletedCount) {
          adjacencyList->deleted = true;
          List_insertInComing(&noInDegNodes, adjacencyList);
        }
      }

      adjacencyList = adjacencyList->next;
    }

    // We can advance to process the next node with indegree of zero.
    Edge *tmp = noInDegNodes->next;
    free(noInDegNodes);
    noInDegNodes = tmp;
  }

  int orderedCount = toCount(topologicalOrder);

  // Reset graph state such as deleted and such.
  resetGraphState(graph);
  cleanupEdgeList(topologicalOrder);
  cleanupEdgeList(noInDegNodes);

  return ((orderedCount != nodesCount) && isConnected);
}

Edge* List_createEdge(nodeStruct *vertex) {
  Edge *edge = malloc(sizeof(struct Edge));
  if (edge != NULL) {
    edge->vertex = vertex;
    edge->next = NULL;
    edge->deleted = false;
  }
  return edge;
}

Edge* List_insertInComing (Edge **incomingList, nodeStruct *target) {
  Edge *found = List_findInComing(*incomingList, target->id);

  if (found == NULL) {
    Edge *edge = List_createEdge(target);

    // Handle empty list
    if (*incomingList == NULL) {
      *incomingList = edge;
      return NULL;
    }
    else {
      // Find the tail and insert node
      Edge *current = *incomingList;
      while (current->next != NULL) {
        current = current->next;
      }
      current->next = edge;
      return current;
    }
  }

  return NULL;
}

Edge* List_insertOutGoing (Edge **outgoingList, nodeStruct *target) {
  Edge *found;

  found = List_findOutGoing(*outgoingList, target->id);

  if (found == NULL) {
    Edge *edge;
    edge = List_createEdge(target);

    // Handle empty list
    if (*outgoingList == NULL) {
      *outgoingList = edge;
      return NULL;
    }
    else {
      // Find the tail and insert node
      Edge *current = *outgoingList;
      while (current->next != NULL) {
        current = current->next;
      }
      current->next = edge;
      return current;
    }
  }

  return NULL;
}

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(long _id)
{
  struct nodeStruct *pNode = malloc(sizeof(struct nodeStruct));
  if (pNode != NULL) {
    pNode->id = _id;
    pNode->next = NULL;
    pNode->deleted = false;
    pNode->incomingList = NULL;
    pNode->outgoingList = NULL;
  }
  return pNode;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node)
{
  node->next = *headRef;
  *headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
nodeStruct* List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node)
{
  node->next = NULL;

  // Handle empty list
  if (*headRef == NULL) {
    *headRef = node;
    return NULL;
  }
  else {
    // Find the tail and insert node
    struct nodeStruct *current = *headRef;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = node;
    return current;
  }
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head)
{
  int count = 0;
  struct nodeStruct *current = head;
  while (current != NULL) {
    current = current->next;
    count++;
  }
  return count;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, long _id)
{
  struct nodeStruct *current = head;
  while (current != NULL) {
    if (current->id == _id) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

Edge* List_findInComing(Edge *head, long _id) {
  Edge *current = head;

  while (current != NULL) {
    if (current->vertex->id == _id) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

Edge* List_findOutGoing(Edge *head, long _id) {
  Edge *current = head;

  while (current != NULL) {
    if (current->vertex->id == _id) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node)
{
  assert(headRef != NULL);
  assert(*headRef != NULL);

  // Is it the first element?
  if (*headRef == node) {
    *headRef = node->next;
  }
  else {
    // Find the previous node:
    struct nodeStruct *previous = *headRef;
    while (previous->next != node) {
      previous = previous->next;
      assert(previous != NULL);
    }

    // Unlink node:
    assert(previous->next == node);
    previous->next = node->next;
  }

  free(node);
}

void List_deleteInComing (Edge **incomingList, nodeStruct *node)
{
  assert(incomingList != NULL);
  assert(*incomingList != NULL);

  Edge *edge = List_findInComing(*incomingList, node->id);

  // Is it the first element?
  if (*incomingList == edge) {
    *incomingList = (*incomingList)->next;
  }
  else {
    // Find the previous node:
    Edge *previous = *incomingList;

    while (previous->next != edge) {
      previous = previous->next;
      assert(previous != NULL);
    }

    // Unlink node:
    assert(previous->next == edge);
    previous->next = edge->next;
  }

  free(edge);
}

void List_deleteOutGoing (Edge **outgoingList, nodeStruct *node)
{
  assert(outgoingList != NULL);
  assert(*outgoingList != NULL);

  Edge *edge = List_findOutGoing(*outgoingList, node->id);

  // Is it the first element?
  if (*outgoingList == edge) {
    *outgoingList = (*outgoingList)->next;
  }
  else {
    // Find the previous node:
    Edge *previous = *outgoingList;

    while (previous->next != edge) {
      previous = previous->next;
      assert(previous != NULL);
    }

    // Unlink node:
    assert(previous->next == edge);
    previous->next = edge->next;
  }

  free(edge);
}

void cleanupEdgeList(Edge *node) {
  Edge *current = node;

  while(current != NULL) {
    Edge *tmp = current->next;
    free(current);
    current = tmp;
  }
}
