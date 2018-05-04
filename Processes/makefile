CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi -g -D_XOPEN_SOURCE=500 -pthread -lrt
EXEC = readersWriters
OBJ = readersWriters.o fileIO.o dataBuffer.o

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -g -D_XOPEN_SOURCE=500 -pthread -lrt

readersWriters.o : readersWriters.c readersWriters.h
	$(CC) -c readersWriters.c $(CFLAGS)

fileIO.o : fileIO.c fileIO.h
	$(CC) -c fileIO.c $(CFLAGS)

dataBuffer.o : dataBuffer.c dataBuffer.h
	$(CC) -c dataBuffer.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
