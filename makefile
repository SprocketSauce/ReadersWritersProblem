CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -g -pthread
EXEC = readersWriters
OBJ = circularBuffer.o fileIO.o dataBuffer.o

#$(EXEC) : $(OBJ)
#	$(CC) $(OBJ) -o $(EXEC) -g

circularBuffer.o : circularBuffer.c circularBuffer.h
	$(CC) -c circularBuffer.c $(CFLAGS)

fileIO.o : fileIO.c fileIO.h
	$(CC) -c fileIO.c $(CFLAGS)

dataBuffer.o : dataBuffer.c dataBuffer.h
	$(CC) -c dataBuffer.c $(CFLAGS)

pthreadTest2 : pthreadTest2.o fileIO.o
	$(CC) pthreadTest2.o fileIO.o -o pthreadTest2 -g -pthread

pthreadTest2.o : pthreadTest2.c
	$(CC) -c pthreadTest2.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ) pthreadTest2 pthreadTest2.o
