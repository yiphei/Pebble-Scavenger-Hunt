# Makefile for Game Server
# GREP, CS50, May 2017

PROG = gameserver
OBJS = gameserver.o ../common/network.o ../common/message.o ../common/log.o ../common/team.o  ../common/krag.o ../common/word.o
LIBS = -lm
LLIBS = ../libcs50/libcs50.a-Linux

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../common/ -I../libcs50/
CC = gcc
MAKE = make

all: $(PROG)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

gameserver.o: ../common/network.h ../libcs50/set.h ../common/log.h ../common/message.h ../libcs50/memory.h ../common/team.h
network.o: ../common/network.h
message.o: ../common/message.h 
log.o: ../common/log.h ../common/network.h
team.o: ../common/team.h ../common/network.h ../libcs50/hashtable.h ../libcs50/set.h ../common/krag.h ../libcs50/file.h
krag.o: ../common/krag.h ../common/team.h
word.o: ../common/word.h

.PHONY: all test valgrind clean 

test:
	gameserver gameId=FEED kff=testKFF sf=testSF port=36587

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all gameserver gameId=FEED kff=testKFF sf=testSF port=36587

clean:
	rm -f gameserver
	rm -f *.o
	rm -f core
	rm -f vgcore