#   Author: Bizuwork Melesse
#   USC Spring 2018 - CS402
#   Assignment: Warmup 2
#
warmup2: parent.o arrival_thread.o error.o my402list.o 
	gcc -o warmup2 -g parent.o arrival_thread.o error.o my402list.o -pthread

parent.o: parent.c parent.h
	gcc -g -c -Wall parent.c parent.h

arrival_thread.o: arrival_thread.c arrival_thread.h 
	 gcc -g -c -Wall arrival_thread.c arrival_thread.h

error.o: error.c error.h 
	 gcc -g -c -Wall error.c error.h
	 
my402list.o: my402list.c my402list.h 
	 gcc -g -c -Wall my402list.c my402list.h

clean:
	rm -f *.o *.h.gch warmup2

