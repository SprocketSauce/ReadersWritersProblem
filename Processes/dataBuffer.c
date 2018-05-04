#define _GNU_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dataBuffer.h"

DataBuffer* createBuffer( int length, int readers )
{
	int i, fd;
	DataBuffer* buff;

	fd = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );
	ftruncate( fd, sizeof( DataBuffer ) );
	buff = (DataBuffer*)mmap( NULL, sizeof( DataBuffer ), PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, 0 );
	
	buff -> buffSM = fd;
	buff -> length = length;
	buff -> index = 0;
	buff -> readers = readers;

	/* Create shared memory segments */
	buff -> arraySM = shm_open( "array", O_CREAT | O_RDWR, 0666 );
	buff -> trackSM = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );

	/* Truncate shared memory segments */
	ftruncate( buff -> arraySM, sizeof(int) * length );
	ftruncate( buff -> trackSM, 80 );

	/* Map shared memory segments */
	buff -> array = (int*)mmap( NULL, sizeof(int) * length, PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, buff -> arraySM, 0 );
	buff -> tracker = (int*)mmap( NULL, sizeof(int), PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, buff -> trackSM, 0 );

	/* Buffer cells that have not been written to have a tracker value of -1 */
	for ( i = 0; i < length; i++ )
	{
		buff -> tracker[i] = -1;
	}

	return buff;
}

void writeTo( DataBuffer* buff, int value )
{
	buff -> tracker[buff -> index] = 0;
	buff -> array[buff -> index] = value;
	buff -> index++;
	if ( buff -> index == buff -> length )
	{
		buff -> index = 0;
	}
}

int isFull( DataBuffer* buff )
{
	return buff -> tracker[buff -> index] != -1 && buff -> tracker[buff -> index] != buff -> readers;
}

void freeBuffer( DataBuffer* buff )
{
	close( buff -> arraySM );
	close( buff -> trackSM );
	close( buff -> buffSM );
}
