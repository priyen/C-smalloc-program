#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

/* Functions to print the datastructures used by smalloc */
extern void *mem;
extern struct block *allocated_list;
extern struct block *freelist;

/* Prints each element of the list using the format string given below:*/
void print_list(struct block *list) {
	struct block *temp = list;
	while(temp != NULL){
		printf("    [addr: %p, size: %d]\n", temp->addr, temp->size );
		temp = temp->next;
	}
    printf("\n");
}

void print_allocated() {
    print_list(allocated_list);
}

void print_free() {
    print_list(freelist);
}

/* write value size times to memory starting at ptr */
void write_to_mem(int size, char *ptr, char value) {
    int i = 0;
    for(i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

/* Prints the contents of allocated memory. Each byte is printed as two
 * hexadecimal digits. */
 //THE FOLLOWING FUNCTION's AUTHOR IS USER: shrest11 of the UTM CS Forum.
 //https://mcs.utm.utoronto.ca/~mybb/utmcs/
 //https://mcs.utm.utoronto.ca/~mybb/utmcs/showthread.php?tid=2901
void print_mem() {
    struct block *cur = allocated_list;

    // Bytes per segment: HALF of the desired number of bytes per line.
    int bps = 8;

    int i, j, i_bound, j_bound;
    while(cur != NULL) {
        printf("%p: size = %d\n", cur->addr, cur->size);

        i_bound = (cur->size / bps) + ((cur->size % bps == 0) ? 0 : 1);
        for(i = 0; i < i_bound; i++) {
            if(i % 2 == 0) {
                printf("%5d:  ", i * bps);
            } else {
                printf("  ");
            }

            // Make sure we don't try to read more than the block's size.
            j_bound = (i < cur->size / bps) ? bps : cur->size % bps;
            for(j = 0; j < j_bound; j++) {
                printf("%02x ", *((char *) cur->addr + ((i * bps) + j)));
            }
            if((i + 1) % 2 == 0 && i < i_bound - 1) {
                printf("\n");
            }
        }

        printf("\n");
        cur = cur->next;
    }
    printf("\n");
}
