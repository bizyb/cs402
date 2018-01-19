warmup1: warmup1.o my402list.o error.o transaction.o history.o
	gcc -o warmup1 -g warmup1.o my402list.o error.o transaction.o history.o

warmup1.o: warmup1.c my402list.h error.h
	gcc -g -c -Wall warmup1.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

error.o: error.c error.h 
	 gcc -g -c -Wall error.c error.h

transaction.o: transaction.c transaction.h
	gcc -g -c -Wall transaction.c

history.o: history.c history.h
	gcc -g -c -Wall history.c



clean:
	rm -f *.o *.h.gch warmup1

