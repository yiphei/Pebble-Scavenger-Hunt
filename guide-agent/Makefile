# Makefile for Guide Agent
# GREP, CS50, May 2017

PROG = guideagent
OBJS = guideagent.o ../common/network.o ../libcs50/set.o ../libcs50/memory.o ../common/message.o ../common/log.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../common/ -I../libcs50/
CC = gcc
MAKE = Makefile

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

guideagent.o: ../common/network.h ../libcs50/set.h
network.o: ../common/network.h
set.o: ../libcs50/set.h ../libcs50/memory.h
message.o: ../common/message.h
log.o: ../common/log.h ../common/network.h

clean:
	rm -f guideagent
	rm -f *.o
	rm -f core
	rm -f vgcore