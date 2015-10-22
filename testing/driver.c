#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64
#define MAXOPS 100

void handle_seg(int code) {
	fprintf(stderr, "SEG FAULT\n");
	exit(1);
}

struct trace_op {
	char type; // 'm' or 'f'
	unsigned int index; // for sfree to use later
	unsigned int size;
};

struct trace_op trace[MAXOPS];
int num_ops;
extern void *mem;
extern struct block *allocated_list;
extern struct block *freelist;

unsigned int total_space(struct block *list) {
	int sum = 0;
    struct block *cur = list;
	while(cur != NULL) {
		sum += cur->size;
		cur = cur->next;
	}
	return sum;
}
unsigned int num_blocks(struct block *list) {
	int count = 0;
    struct block *cur = list;
	while(cur != NULL) {
		count ++;
		cur = cur->next;
	}
	return count;
}


void load_trace(FILE *fp) {
	int i = 0;
	char type[10];
	num_ops = 0;
	while((fscanf(fp, "%s", type)) != EOF) {
		switch (type[0]) {
			case 'm':
				trace[i].type = type[0];
				fscanf(fp, "%u %u", &trace[i].index, &trace[i].size);
				num_ops++;
			break;
			case 'f':
				trace[i].type = type[0];
				fscanf(fp, "%u", &trace[i].index);
				num_ops++;
			break;
			default:
				fprintf(stderr, "Bad type (%c) in trace file\n", type[0]);
				exit(1);
		}
		i++;
	}

}




/* Simple test for malloc and free. We will want a proper test driver */
int main(int argc, char **argv) {

	int opt;
	int pr = 0;

    struct sigaction newact;

    /* fill in newact */
	newact.sa_handler = handle_seg;
    newact.sa_flags = 0;
	sigaction(SIGSEGV, &newact, NULL);

	while ((opt = getopt(argc, argv, "p")) != -1) {
		switch (opt) {
		case 'p':
			pr = 1;
		break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-p ] tracefile\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if(optind >= argc) {
		fprintf(stderr, "Usage: %s [-p ] tracefile\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *tracefile = argv[optind];

    mem_init(SIZE);
    
    char *ptrs[MAXOPS];
    int i;


    /* Call my malloc 4 times */
    
        //ptrs[i] = smalloc(num_bytes);
        //write_to_mem(num_bytes, ptrs[i], i);
	FILE *fp = fopen(tracefile, "r");
	if(!fp) {
		fprintf(stderr, "Usage: driver [-p] <tracefile>\n");
		exit(1);
	}
	load_trace(fp);
	fclose(fp);

	for(i = 0; i < num_ops; i++) {
		switch(trace[i].type) {
	    case 'm':
			ptrs[trace[i].index] = smalloc(trace[i].size);

			if(pr) {
		    	printf("smalloc block %d addr %p size %d\n", 
					trace[i].index, ptrs[trace[i].index], trace[i].size);
			}

			if(!ptrs[trace[i].index]) {
			    fprintf(stderr, "Error: smalloc returned NULL index=%d, size=%d\n", 
					trace[i].index, trace[i].size);
		    }
			// check for valid pointer
			if((ptrs[trace[i].index] < (char *)mem) || 
				(ptrs[trace[i].index] + trace[i].size) > ((char *)mem + SIZE)) {
			   fprintf(stderr, "Error: malloc block %d addr %p size %d heap overflow\n", 
					trace[i].index, ptrs[trace[i].index], trace[i].size);
		    } else {
               write_to_mem(trace[i].size, ptrs[trace[i].index],trace[i].index);
			}
	        break;
	    case 'f':
			if(pr) {
				printf("sfree block %d addr %p\n", 
					 trace[i].index, ptrs[trace[i].index]);
			}
			if(sfree(ptrs[trace[i].index])) {
				fprintf(stderr, "Error: free block %d addr %p\n", 
					trace[i].index, ptrs[trace[i].index]);
			}
				
	        break;
		default :
			fprintf(stderr, "ERROR: bad type of instruction %c for block %d\n",
					trace[i].type, trace[i].index);
	    }
	}

	if(pr) {
		printf("Total free mem: %d\n", total_space(freelist));
		printf("Total allocated mem: %d\n", total_space(allocated_list));
   	 	printf("List of allocated blocks:\n");
		print_allocated();
    	printf("List of free blocks:\n");
    	print_free();
    	printf("Contents of allocated memory:\n");
    	print_mem();
	} else {
		printf("Total free mem: %d\n", total_space(freelist));
		printf("Total allocated mem: %d\n", total_space(allocated_list));
		printf("Number of allocated blocks: %d\n", num_blocks(allocated_list));
	}


    mem_clean();
    return 0;
}
