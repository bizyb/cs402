#   Author: Bizuwork Melesse
#   USC Spring 2018 - CS402
#   Assignment: Warmup 2
#
warmup2: parent.o error.o
	gcc -o warmup2 -g parent.o error.o

parent.o: parent.c parent.h
	gcc -g -c -Wall parent.c

error.o: error.c error.h 
	 gcc -g -c -Wall error.c error.h

clean:
	rm -f *.o *.h.gch warmup2

