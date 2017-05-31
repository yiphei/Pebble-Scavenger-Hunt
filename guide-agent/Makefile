# Makefile for Guide Agent
# GREP, CS50, May 2017

PROG = guideagent
DISPLAY = displaytest

OBJS = guideagent.o ../common/network.o ../common/message.o ../common/log.o ../common/team.o display.o ../common/krag.o ../common/word.o ../common/word.o ../common/file.h
OBJS2 = ../common/network.o ../common/message.o ../common/log.o ../common/team.o display.o displaytest.o ../common/krag.o ../common/file.o ../common/word.o
LIBS = -lm -lncurses
LLIBS = ../libcs50/libcs50.a-Linux

CFLAGS = -Wall -pedantic -std=c11 -ggdb -DNOPEBBLE -I../common/ -I../libcs50/
CC = gcc
MAKE = make

all: $(PROG) $(DISPLAY)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

$(DISPLAY): $(OBJS2) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

display.o: display.h ../common/team.h ../libcs50/set.h
displaytest.o: display.h ../common/team.h ../libcs50/set.h ../libcs50/hashtable.h ../common/word.h
guideagent.o: ../common/network.h ../libcs50/set.h ../common/file.h ../common/word.h ../common/log.h ../common/message.h ../libcs50/memory.h ../common/team.h display.h
file.o: ../common/file.h
message.o: ../common/message.h ../common/word.h
network.o: ../common/network.h
log.o: ../common/log.h ../common/network.h
team.o: ../common/team.h ../common/network.h ../libcs50/hashtable.h ../libcs50/set.h ../common/krag.h ../libcs50/file.h
krag.o: ../common/krag.h ../common/team.h
word.o: ../common/word.h

.PHONY: all clean display test

test:
	guideagent guideId=9876 player=Tony team=One host=moose.cs.dartmouth.edu port=36587

display: $(DISPLAY)
	./$(DISPLAY)

clean:
	rm -f guideagent
	rm -f *.o
	rm -f core
	rm -f vgcore
