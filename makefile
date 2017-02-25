# $Id: makefile,v 1.1.1.1 2012/05/04 23:44:35 bediger Exp $
CC = cc
CFLAGS = -I. -g -Wall

# For gcov coverage testing:
#CFLAGS = -I. -g -Wall -fprofile-arcs -ftest-coverage

OBJS = tree.o buffer.o cb.o aho_corasick.o algorithm_d.o

node_counter: node_counter.o tree.o
	$(CC) $(CFLAGS) -o node_counter node_counter.o tree.o

test6: main.o $(OBJS)
	$(CC) $(CFLAGS) -o test6 main.o $(OBJS)

main.o: main.c tree.h buffer.h aho_corasick.h algorithm_d.h
	$(CC) $(CFLAGS) -c -o main.o main.c

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -c -o tree.o tree.c

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c -o buffer.o buffer.c

cb.o: cb.c cb.h
	$(CC) $(CFLAGS) -c -o cb.o cb.c

aho_corasick.o: aho_corasick.c aho_corasick.h cb.h
	$(CC) $(CFLAGS) -c -o aho_corasick.o aho_corasick.c

algorithm_d.o: algorithm_d.c tree.h algorithm_d.h aho_corasick.h
	$(CC) $(CFLAGS) -c -o algorithm_d.o algorithm_d.c

clean:
	-rm -rf *.a *.o *core test?
	-rm -rf *.gcno *.gcda
	-rm -rf node_counter
