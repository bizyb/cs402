warmup1: warmup1.o my402list.o error.o
	gcc -o warmup1 -g warmup1.o my402list.o error.o

warmup1.o: warmup1.c my402list.h error.h cs402.h
	gcc -g -c -Wall warmup1.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

error.o: error.c error.h cs402.h
	 gcc -g -c -Wall error.c error.h cs402.h

clean:
	rm -f *.o warmup1

