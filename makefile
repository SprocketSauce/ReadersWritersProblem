CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -g -pthread
EXEC = readersWriters
OBJ = readersWriters.o fileIO.o dataBuffer.o

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -g -pthread

readersWriters.o : readersWriters.c readersWriters.h
	$(CC) -c readersWriters.c $(CFLAGS)

fileIO.o : fileIO.c fileIO.h
	$(CC) -c fileIO.c $(CFLAGS)

dataBuffer.o : dataBuffer.c dataBuffer.h
	$(CC) -c dataBuffer.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
