CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -g -pthread
EXEC = readersWriters
OBJ = circularBuffer.o

#$(EXEC) : $(OBJ)
#	$(CC) $(OBJ) -o $(EXEC) -g

circularBuffer.o : circularBuffer.c circularBuffer.h
	$(CC) -c circularBuffer.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
