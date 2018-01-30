all: list parser

sus: src/main.c src/GEDCOMparser.c src/LinkedListAPI.c
	gcc -Wall -g -std=c11 src/main.c src/GEDCOMparser.c src/LinkedListAPI.c -Iinclude

list: src/LinkedListAPI.c include/LinkedListAPI.h
	gcc -Wall -g -std=c11 -Iinclude -c src/LinkedListAPI.c -o bin/LinkedListAPI.o;\
	ar cr bin/liblist.a bin/LinkedListAPI.o

parser: src/GEDCOMparser.c include/GEDCOMparser.h
	gcc -Wall -g -std=c11 -Iinclude -c src/GEDCOMparser.c -o bin/GEDCOMparser.o;\
	ar cr bin/libparse.a bin/GEDCOMparser.o

clean:
	rm bin/*.o bin/*.a *.o *.a include/*.o include/*.a src/*.o src/*.a
