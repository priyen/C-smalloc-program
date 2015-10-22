all : simpletest mytest

simpletest : simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o
	
mytest : mytest.o smalloc.o testhelpers.o
	gcc -Wall -g -o mytest mytest.o smalloc.o testhelpers.o
	
tests: simpletest mytest
	./simpletest
	./mytest

%.o : %.c smalloc.h
	gcc -Wall -g -c $<
	
clean : 
	rm simpletest mytest *.o
