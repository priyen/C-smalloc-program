all : simpletest driver testerrors testinit

simpletest : simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o

driver : driver.o smalloc.o testhelpers.o
	gcc -Wall -g -o driver driver.o smalloc.o testhelpers.o
testerrors : testerrors.o smalloc.o testhelpers.o
	gcc -Wall -g -o testerrors testerrors.o smalloc.o testhelpers.o
testinit : testinit.o smalloc.o testhelpers.o
	gcc -Wall -g -o testinit testinit.o smalloc.o testhelpers.o

	
%.o : %.c smalloc.h
	gcc -Wall -g -c $<
	
clean : 
	rm simpletest *.o driver testerrors *.out
