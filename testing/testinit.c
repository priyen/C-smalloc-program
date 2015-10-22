#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64


extern struct block *freelist;
extern struct block *allocated_list;
extern void *mem;

// A test for errors
int main() {

    mem_init(SIZE);

	if(allocated_list != NULL) {
	 	printf("Allocated list is not empty or is not initialized\n");
		exit(1);
	}
	if(freelist->addr != mem || freelist->size != SIZE) {
		printf("First node of free list has bad values\n");
		exit(1);
	}
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    return 0;
}
