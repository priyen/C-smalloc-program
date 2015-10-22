#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64




// A test for errors
int main() {

    mem_init(SIZE);
    
    char *ptrs[10];

    
    ptrs[0] = smalloc(64);
    write_to_mem(64, ptrs[0], 6);
    ptrs[1] = smalloc(64);
    write_to_mem(64, ptrs[1], 6);
    ptrs[2] = smalloc(64);
    write_to_mem(64, ptrs[2], 6);

	int result = sfree(NULL);
	if(result  != -1) {
		printf("ERROR: Free of NULL did not return -1 (%d)\n", result);
		exit(1);
	}

	result = sfree(ptrs[1]);
	if(result  != 0 ){
		printf("ERROR: Free of allocated memory did not return 0 (%d)\n", result);
		exit(1);
	}
	
	result = sfree(ptrs[1]);
	if(result  != -1) {
		printf("ERROR: Double free did not return -1 (%d)\n", result);
		exit(1);
	}
    /*
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
	*/

    mem_clean();
    return 0;
}
