#   Author: Bizuwork Melesse
#   USC Spring 2018 - CS402
#   Assignment: Warmup 2
#
warmup2: parent.o
	gcc -o warmup2 -g parent.o

# warmup2: warmup2.o my402list.o error.o transaction.o history.o
# 	gcc -o warmup2 -g warmup2.o my402list.o error.o transaction.o history.o


parent.o: parent.c
	gcc -g -c -Wall parent.c

# parent.o: parent.c my402list.h error.h
# 	gcc -g -c -Wall parent.c

# my402list.o: my402list.c my402list.h
# 	gcc -g -c -Wall my402list.c

# error.o: error.c error.h 
# 	 gcc -g -c -Wall error.c error.h

# transaction.o: transaction.c transaction.h
# 	gcc -g -c -Wall transaction.c

# history.o: history.c history.h
# 	gcc -g -c -Wall history.c



clean:
	rm -f *.o *.h.gch warmup2

