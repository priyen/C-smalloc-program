#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {	
	//If no freelist, nothing to allocate
	if (!freelist){
		return NULL;
	}
	//Temp pointer to freelist
	//Prev to keep track of the previous block / 'parent'
	struct block *temp = freelist;
	struct block *prev = NULL;
	while(temp){
		//Exact size block, no need to split since its the perfect size matching nbytes
		if(temp->size == nbytes){
			//Is allocated NULL?
			if(allocated_list){
				if(prev){
					prev->next = temp->next;
				}
				else{
					freelist = temp->next;					
				}
				temp->next = allocated_list;
				allocated_list = temp;
			}
			else{//Allocated is initialized but NULL
				if(prev){
					prev->next = temp->next;
				}
				else{
					freelist = temp->next;
				}
				allocated_list = temp;
				temp->next = NULL;
			} 
			return allocated_list->addr;
		}//Not exact size
		if(temp->size >= nbytes){
			//Is allocated NULL?
			if(allocated_list){
				//Make a new block with that size, split the original bigger block
				struct block *new = malloc(sizeof(struct block));
				new->size = nbytes;
				new->addr = temp->addr;
				temp->addr = temp->addr + nbytes;
				new->next = allocated_list;
				temp->size = temp->size - nbytes;
				allocated_list = new;
			}
			else{//Allocated is initialized but NULL
				//Make a new block with that size, split the original bigger block
				allocated_list = malloc(sizeof(struct block));
				allocated_list->size = nbytes;
				allocated_list->addr = temp->addr;
				allocated_list->next = NULL;
				temp->addr = temp->addr + nbytes;
				temp->size = temp->size - nbytes;
			}
			return allocated_list->addr;
		}	  
		prev = temp;
		temp = temp->next;
	}
	return NULL;  
}


int sfree(void *addr) {
	//If no allocated_list, nothing to free
	if(!allocated_list){
		return -1;
	}
	//Temp pointer to allocated_list
	//Prev to keep track of the previous block / 'parent'
	struct block *temp = allocated_list;
	struct block *prev = NULL;
	while(temp){
		//Does block match addr?
		if(temp->addr == addr){
			//Is it the first block?
			if(prev){
				prev->next = temp->next;
			}
			else{//If it isn't first block in the list
				allocated_list = temp->next;
			}
			temp->next = freelist;
			freelist = temp;
			return 0;
		}
		prev = temp;
		temp = temp->next;
	}
	return -1;
}


/* Initialize the memory space used by smalloc,
* freelist, and allocated_list
* Note:  mmap is a system call that has a wide variety of uses.  In our
* case we are using it to allocate a large region of memory. 
* - mmap returns a pointer to the allocated memory
* Arguments:
* - NULL: a suggestion for where to place the memory. We will let the 
*         system decide where to place the memory.
* - PROT_READ | PROT_WRITE: we will use the memory for both reading
*         and writing.
* - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
*         is not associated with a file.
* - -1: because this memory is not associated with a file, the file 
*         descriptor argument is set to -1
* - 0: only used if the address space is associated with a file.
*/
void mem_init(int size) {
	mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if(mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	//Initialize but set to NULL
	allocated_list = NULL;

	//Make the first freelist block
	freelist = malloc(sizeof(struct block));
	freelist->addr = mem;
	freelist->size = size;
	freelist->next = NULL;
}

void mem_clean(){	
	//If there is an allocated_list, free each block
	if(allocated_list){
		struct block *prev;
		while(allocated_list){
			prev = allocated_list;
			allocated_list = allocated_list->next;
			free(prev);
		}
	}
	
	//If there is an freelist, free each block
	if(freelist){
		struct block *prev;
		while(freelist){
			prev = freelist;
			freelist = freelist->next;
			free(prev);
		}
	}
}

