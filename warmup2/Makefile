#   Author: Bizuwork Melesse
#   USC Spring 2018 - CS402
#   Assignment: Warmup 2
#
warmup2: parent.o arrival_thread.o error.o my402list.o token_thread.o server_thread.o stats.o monitor.o
	gcc -o warmup2 -g parent.o arrival_thread.o error.o my402list.o token_thread.o server_thread.o stats.o monitor.o -pthread -lm

parent.o: parent.c parent.h
	gcc -g -c -Wall parent.c parent.h 

arrival_thread.o: arrival_thread.c arrival_thread.h 
	 gcc -g -c -Wall arrival_thread.c arrival_thread.h

token_thread.o: token_thread.c token_thread.h 
	 gcc -g -c -Wall token_thread.c token_thread.h

server_thread.o: server_thread.c server_thread.h 
	 gcc -g -c -Wall server_thread.c server_thread.h

stats.o: stats.c stats.h 
	 gcc -g -c -Wall stats.c stats.h

error.o: error.c error.h 
	 gcc -g -c -Wall error.c error.h

monitor.o: monitor.c monitor.h 
	 gcc -g -c -Wall monitor.c monitor.h

my402list.o: my402list.c my402list.h 
	 gcc -g -c -Wall my402list.c my402list.h

clean:
	rm -f *.o *.h.gch warmup2

