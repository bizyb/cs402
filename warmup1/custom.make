functiontest: functiontest.o my402list.o
	gcc -o functiontest -g functiontest.o my402list.o

functiontest.o: functiontest.c my402list.h
	gcc -g -c -Wall functiontest.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o functiontest