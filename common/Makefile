# Makefile for 'common'
#
# Yifei Yan

PROG = commontest
OBJS = krag.o team.o commontest.o
LIB = common.a
LIBS = -lm -lcurl
LLIBS = ../libcs50/libcs50.a-Linux

# uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

FLAGS = # -DNOSLEEP # -DMEMTEST 

CFLAGS = -Wall -pedantic -std=gnu11 -ggdb $(FLAGS)
CFLAGS2 = -Wall -pedantic -std=gnu11 -ggdb $(TESTING) -I../common
CC = gcc
MAKE = make


# Build the library by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS) $(LLIBS)


$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS2) $^ $(LLIBS) $(LIBS) -o $@

# Dependencies: object files depend on header files
krag.o: krag.h team.h
team.o: team.h krag.h network.h
commontest.o: krag.h team.h

.PHONY: clean sourcelist


test: $(PROG)
	./$(PROG) kragfile secretfile



# list all the sources and docs in this directory.
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f $(LIB) *~ *.o



#gcc -c krag.h
