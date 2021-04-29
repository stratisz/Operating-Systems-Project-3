all: chef saladmaker

chef: chef.o
	gcc chef.o -o chef -lpthread

saladmaker: saladmaker.o
	gcc saladmaker.o -o saladmaker -lpthread

chef.o: chef.c
	gcc -c chef.c -lpthread

worker.o: worker.c
	gcc -c chef.c -lpthread

clean:
	rm *o saladmaker chef