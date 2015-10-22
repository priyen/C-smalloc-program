#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#define SIZE 4096 * 64


/* My test for smalloc and sfree. 
The test case this file tests what happens when all allocated blocks are
freed? Specifically, what happens to the pointer of the allocated list.
Why is this an interesting case? Well we know initially, the allocated_list
struct block is initialized, but the pointer is NULL; smalloc has specific
code that handles the scenario where allocated_list is empty. 
Without a special case when using sfree that handles when sfree is called 
on the only left allocated block, the program will add that 'allocated block' 
to the freelist, in doing so, the pointer to any 'allocated_list' itself is lost 
because the only one block  was just 'transferred' / added to freelist. The ideal way to correct for this 
or prevent it is simply to initialize a struct block for allocated_list and set the pointer to NULL.
This is similar to how the program started via mem_init where allocated_list had to be initialized.
Ideally, this should be done in sfree when the only 'allocated block' is freed.

Furthermore, what happens when the final free block is used? The same situation described
above applies to freelist when you want to allocate the only block in the list, and both these cases are tested. 

How are these cases tested?
The program below allocates all memory to one block, then free's that block.
The program should be able to re-initialize the lists to empty in these circumstances.

***What would happen if you DO NOT account for this?***
List of allocated blocks:
    [addr: 0x7f3a69e66000, size: 262144]

List of free blocks:
    [addr: 0x7f3a69e66000, size: 262144]
	
	
If this is not accounted for, the same block will end up in both lists because both lists will point to the same
block. This would essentially be corrupting memory as you can't have memory that is free and allocated at the same
location, they should be mutually exclusive. Accordingly this test case is important to consider.

***What should happen?***
List of allocated blocks:

List of free blocks:
    [addr: 0x7f3a69e66000, size: 262144]
	
----------	Allocate all memory
	|
	|
	v 
	
List of allocated blocks:
    [addr: 0x7f3a69e66000, size: 262144]

List of free blocks:
	
---------- Free that block
	|
	|
	v 
	
List of allocated blocks:

List of free blocks:
    [addr: 0x7f3a69e66000, size: 262144]
	
Note to the TA: The code to account for the above test cases is already added to sfree 
(for allocated_list case) and smalloc (for freelist case) by me, if you want to see what happens, 
simply comment out lines the appropriate lines in smalloc.c, make clean, make all, run mytest and 
you will see why this test case is important/the behaviour of the program if it is not accounted for.
 */

int main(void) {

    mem_init(SIZE);

    char *ptr;

    /*Allocating all memory */
    //Test case for smalloc use up all memory and allocate it in 1 block so we get a empty freelist
	printf("Allocating all memory using 1 block\n");
    ptr = smalloc(SIZE);
    write_to_mem(SIZE, ptr, 0);
	
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    printf("Free all memory from that 1 block\n");
    /*Test case for allocated_list. Free the only allocated block we have*/
    printf("freeing %p result = %d\n", ptr, sfree(ptr));
   
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    mem_clean();
    return 0;
}
