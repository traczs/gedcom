CC=gcc
CFLAGS= -Wall -g -std=c11 -Iinclude

#UNAME Shell Variable
UNAME_S := $(shell uname -s)

# OS X Dependant Variables
ifeq ($(UNAME_S), Darwin)
    LIB := sharedLib.dylib
# GNU/Linux Depedant Variables
else ifeq ($(UNAME_S), Linux)
    LIB := sharedLib.so
endif

all: $(LIB) sharedLib.o

$(LIB): sharedLib.o
	$(CC) -shared -o ../$(LIB) GEDCOMparser.o LinkedListAPI.o

sharedLib.o: src/GEDCOMparser.c src/LinkedListAPI.c
	$(CC) $(CFLAGS)  -c -fpic src/GEDCOMparser.c src/LinkedListAPI.c

clean:
	rm ../$(LIB) *.o
