#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kallocator.h"

// Provided from instructor's solution
#include "list.h"

static _Bool is_empty(nodeStruct *node);

struct KAllocator {
	enum allocation_algorithm aalgorithm;
	int size;
	int allocated_size;

	void* memory;

	// Some other data members you want,
	// such as lists to record allocated/free memory
	nodeStruct *free_list;
	nodeStruct *allocated_list;
};

struct KAllocator kallocator;


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
	assert(_size > 0);
	kallocator.aalgorithm = _aalgorithm;
	kallocator.size = _size;
	kallocator.memory = malloc((size_t)kallocator.size);

	// Add some other initialization
	kallocator.free_list = NULL;
	kallocator.allocated_list = NULL;
	kallocator.allocated_size = 0;

	nodeStruct *freeMemory = List_createNode(kallocator.memory, _size);
	List_insertTail(&(kallocator.free_list), freeMemory);
}

void destroy_allocator() {
	free(kallocator.memory);

	nodeStruct *cursor = kallocator.allocated_list;
	while(cursor != NULL) {
		nodeStruct *tmp = cursor->next;
		free(cursor);
		cursor = tmp;
	}

	cursor = kallocator.free_list;
	while(cursor != NULL) {
		nodeStruct *tmp = cursor->next;
		free(cursor);
		cursor = tmp;
	}
}

void* kalloc(int _size) {
	void* ptr = NULL;

	switch(kallocator.aalgorithm) {
		case FIRST_FIT:
			ptr = kalloc_first_fit(_size);
			break;
		case BEST_FIT:
			ptr = kalloc_best_fit(_size);
			break;
		case WORST_FIT:
			ptr = kalloc_worst_fit(_size);
			break;
		default:
			printf("Error: Unknown allocator algorigthm.\n");
	}

	if (!is_empty(ptr)) {
		kallocator.allocated_size += _size;
	}

	return ptr;
}

void kfree(void* _ptr) {
	assert(_ptr != NULL);

	nodeStruct *found = List_findNode(kallocator.allocated_list, _ptr);
	nodeStruct *freeMemory = List_createNode(found->block, found->size);
	nodeStruct *prevBlock = List_insertTail(&kallocator.free_list, freeMemory);

	// Merge contiguous free blocks
	if (is_mergeable(prevBlock, freeMemory)) {
		List_mergeNodes(prevBlock, freeMemory);
	}

	kallocator.allocated_size -= found->size;
	List_deleteNode(&kallocator.allocated_list, found);
}

/*
* compact_allocation shifts allocated block to
* the begining of the free blocks by swtching their block addresses.
* Note that we do NOT recalculate the size of the remaining free blocks
* once we do any such change of addresses. We just use the first free block
* and keep changing its block address accordingly. Note that
* this might cause free blocks to overlap each other in terms of addresses, but
* this is fine, since the free blocks are getting pushed to the end, and we will
* merge them all together to create one free block.
*/
int compact_allocation(void** _before, void** _after) {
	List_sort(&(kallocator.allocated_list));
	List_sort(&(kallocator.free_list));

	nodeStruct *AB = kallocator.allocated_list;
	nodeStruct *FB = kallocator.free_list;

	int total_swaps = 0;

	// No free blocks ?! Nothing to compact!
	if (is_empty(FB)) {
		return total_swaps;
	}

	int total_size = 0;
	while(!is_empty(AB)) {
		// The first block is an allocated one
		if (AB->block == (char *)(kallocator.memory)+total_size) {
			total_size += AB->size;
		} else {
			// The first block is a free one

			memcpy(FB->block, AB->block, AB->size);

			_before[total_swaps] = AB->block;

			AB->block = FB->block;
			FB->block = AB->block+AB->size;

			_after[total_swaps] = AB->block;

			total_size += AB->size;
			total_swaps += 1;
		}

		AB = AB->next;
	}

	mergify();
	return total_swaps;
}

int available_memory() {
	int available_memory_size = 0;

	// Calculate available memory size
	nodeStruct *cursor = kallocator.free_list;
	while(!is_empty(cursor)) {
		available_memory_size += cursor->size;
		cursor = cursor->next;
	}

	return available_memory_size;
}

void print_statistics() {
	int allocated_chunks = 0;
	int free_size = kallocator.size - kallocator.allocated_size;

	int free_chunks = 0;
	int smallest_free_chunk_size = kallocator.size;
	int largest_free_chunk_size = 0;


	nodeStruct *cursor = kallocator.free_list;
	while(!is_empty(cursor)) {
		free_chunks += 1;

		if (cursor->size < smallest_free_chunk_size) {
			smallest_free_chunk_size = cursor->size;
		}

		if (cursor->size > largest_free_chunk_size) {
			largest_free_chunk_size = cursor->size;
		}

		cursor = cursor->next;
	}

	cursor = kallocator.allocated_list;
	while(!is_empty(cursor)) {
		allocated_chunks += 1;
		cursor = cursor->next;
	}

	// Calculate the statistics
	printf("Allocated size = %d\n", kallocator.allocated_size);
	printf("Allocated chunks = %d\n", allocated_chunks);
	printf("Free size = %d\n", free_size);
	printf("Free chunks = %d\n", free_chunks);
	printf("Largest free chunk size = %d\n", largest_free_chunk_size);
	printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}

void mergify() {
	List_sort(&(kallocator.free_list));

	nodeStruct *memo = kallocator.free_list;
	nodeStruct *cursor = kallocator.free_list->next;

	while(!is_empty(cursor)) {
		List_mergeNodes(memo, cursor);
		cursor = cursor->next;
	}

	// Update the only free node with the remainder free size
	memo->size = kallocator.size - kallocator.allocated_size;
}

_Bool is_mergeable(nodeStruct *prev, nodeStruct *current) {
	if (is_empty(prev) || is_empty(current)) {
		return false;
	}

	return ((prev->block)+prev->size) == (current->block);
}

void print_memory() {
	nodeStruct *current_node = kallocator.free_list;

	printf("FREE LIST: \n");
	while (current_node != NULL) {
		printf("\t[%p: %d] Val: %d\n", current_node->block, current_node->size, *((int *)current_node->block));
		current_node = current_node->next;
	}

	printf("\n");
	printf("ALLOCATED LIST: \n");
	current_node = kallocator.allocated_list;

	while (current_node != NULL) {
		printf("\t[%p: %d] Val: %d\n", current_node->block, current_node->size, *((int *)current_node->block));
		current_node = current_node->next;
	}

	printf("\n");
}

void print_memory_sorted() {
	List_sort(&(kallocator.free_list));
	nodeStruct *current_node = kallocator.free_list;

	printf("FREE LIST: \n");
	while (current_node != NULL) {
		printf("\t[%p: %d] Val: %d\n", current_node->block, current_node->size, *((int *)current_node->block));
		current_node = current_node->next;
	}

	printf("\n");
	printf("ALLOCATED LIST: \n");

	List_sort(&(kallocator.allocated_list));
	current_node = kallocator.allocated_list;

	while (current_node != NULL) {
		printf("\t[%p: %d] Val: %d\n", current_node->block, current_node->size, *((int *)current_node->block));
		current_node = current_node->next;
	}

	printf("\n");
}

void allocate(nodeStruct *mNode, void** _ptr, int _size) {
	nodeStruct *allocatedMemory = List_createNode(mNode->block, _size);
	List_insertTail(&kallocator.allocated_list, allocatedMemory);

	*_ptr = mNode->block;
	mNode->block = ((char *)(mNode->block)+_size);

	if (mNode->size > _size) {
			mNode->size -= _size;
	} else {
			List_deleteNode(&kallocator.free_list, mNode);
	}
}

void* kalloc_worst_fit(int _size) {
	void* ptr = NULL;
	int residual = 0;
	nodeStruct *cursor = kallocator.free_list;
	nodeStruct *worstFit;

	while(!is_empty(cursor)) {
		if (cursor->size >= _size && (cursor->size-_size) >= residual) {
				worstFit = cursor;
				residual = (cursor->size-_size);
		}
		cursor = cursor->next;
	}
	if (is_empty(worstFit)) return ptr;

	allocate(worstFit, &ptr, _size);
	return ptr;
}

void* kalloc_first_fit(int _size) {
	void* ptr = NULL;
	nodeStruct *cursor = kallocator.free_list;

	while (!is_empty(cursor)) {
		if (cursor->size >= _size) {
				break;
		}
		cursor = cursor->next;
	}
	if (is_empty(cursor)) return ptr;

	allocate(cursor, &ptr, _size);
	return ptr;
}

void* kalloc_best_fit(int _size) {
	void* ptr = NULL;
	int residual = kallocator.size;
	nodeStruct *cursor = kallocator.free_list;
	nodeStruct *bestFit = NULL;

	while(!is_empty(cursor)) {
		if (cursor->size >= _size && (cursor->size-_size) <= residual) {
			bestFit = cursor;
			residual = (cursor->size-_size);
		}
		cursor = cursor->next;
	}
	if (is_empty(bestFit)) return ptr;

	allocate(bestFit, &ptr, _size);
	return ptr;
}

static _Bool is_empty(nodeStruct *node) {
	return node == NULL;
}
