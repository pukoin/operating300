#include "list.h"
#include<stddef.h>
#include<stdlib.h>
#include<stdio.h>
void List_insertHead(struct nodeStruct** headRef, struct nodeStruct* node) {
    struct nodeStruct** newHead =  headRef; // new ref to head
    node->next = *headRef;
    *newHead = node;
}

struct nodeStruct* List_createNode(int item)
{    
 
    struct nodeStruct* nd = (struct nodeStruct*)malloc(sizeof(struct nodeStruct));
    nd->item = item;
    nd->next = NULL;
    return nd;
    
}
void List_insertTail (struct nodeStruct** headRef,struct nodeStruct *node)
{
    struct nodeStruct **newHead = headRef;
    while ((*newHead)->next != NULL) {
        *newHead = (*newHead)->next;
    }
    (*newHead)->next = node;
}

struct nodeStruct* List_findNode(struct nodeStruct *head, int item)
{
    struct nodeStruct *temp = head;
    if(temp == NULL) //if list is empty
    {
        return NULL;
    }
  
    while ((temp->item) != item && (temp->next) != NULL){
        temp = temp->next;
    }

    if(temp->item == item){
        return temp;
    }
    
    return NULL;

}

int List_countNodes (struct nodeStruct *head)
{
    struct nodeStruct *temp = head;
    int count = 0;
    while (temp != NULL) {
        count++;

        temp = temp->next;
    }   
    
    return count;
}

void List_deleteNode (struct nodeStruct** headRef, struct nodeStruct* node) {
    struct nodeStruct **newHead = headRef;
    struct nodeStruct *curHead = *headRef;
    struct nodeStruct *prev = *headRef;
    if (curHead == NULL)
        return;

    if (curHead == node) {
        *headRef = node->next;
        return;
    }

    curHead = (curHead)->next;

    while (curHead != NULL) {
        if (curHead == node) {
            prev->next = node->next;
            free(node);
            return;
        }
        prev = prev->next;
        curHead = curHead->next;
    }
}

struct nodeStruct* merge(struct nodeStruct *node1, struct nodeStruct *node2)
{
    struct nodeStruct *newNode = List_createNode(0);
    struct nodeStruct *curr = newNode;

    while(node1 && node2) {
        if (node1->item < node2->item) {
            curr->next = node1;
            node1 = node1->next;
        }
        else {
            curr->next = node2;
            node2 = node2->next;
        }
        curr = curr->next;
    }

    curr->next = (node1 == NULL) ? node2 : node1;
    return newNode->next;


}

struct nodeStruct* merge_sort(struct nodeStruct *headRef){
    struct nodeStruct* newNode = headRef;

    if (!newNode || !(newNode->next)) return newNode;
    struct nodeStruct *slow = newNode, 
                      *fast = newNode, 
                      *pre = newNode;

    while (fast && fast->next) {
        pre = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    pre->next = NULL;
    return merge(merge_sort(newNode), merge_sort(slow));
}

void List_sort(struct nodeStruct **headRef) {
  *headRef = merge_sort(*headRef);

}