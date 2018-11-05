// Provided from instructor's solution
#include "list.h"
#include <stdbool.h>

#ifndef __KALLOCATOR_H__
#define __KALLOCATOR_H__

enum allocation_algorithm {FIRST_FIT, BEST_FIT, WORST_FIT};

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm);

void* kalloc(int _size);
void* kalloc_first_fit(int _size);
void* kalloc_best_fit(int _size);
void* kalloc_worst_fit(int _size);
void allocate(nodeStruct *mNode, void** _ptr, int _size);

void print_statistics();
void print_memory();
void print_memory_sorted();

void kfree(void* _ptr);
int available_memory();
int compact_allocation(void** _before, void** _after);
void destroy_allocator();

void mergify();
_Bool is_mergeable(nodeStruct *prev, nodeStruct *current);

#endif
