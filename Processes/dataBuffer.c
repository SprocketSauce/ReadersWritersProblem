#include <stdlib.h>
#include "dataBuffer.h"

DataBuffer* createBuffer( int length, int readers )
{
	int i, arraySM, trackSM;
	DataBuffer* buff;
	
	buff = malloc( sizeof(DataBuffer) );
	buff -> length = length;
	buff -> index = 0;
	buff -> readers = readers;

	/* Create shared memory segments */
	arraySM = shm_open( "array", O_CREAT | O_RDWR, 0666 );
	trackSM = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );

	/* Truncate shared memory segments */
	ftruncate( arraySM, sizeof(int) * length );
	ftruncate( trackSM, sizeof(int) * length );

	/* Map shared memory segments */
	buff -> array = (int*)mmap( NULL, sizeof(int) * length, PROT_READ | PROT_WRITE, 
		MAP_SHARED,	arraySM, 0 );
	buff -> tracker = (int*)mmap( NULL, sizeof(int) * length, PROT_READ | PROT_WRITE, 
		MAP_SHARED, trackSM, 0 );

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
	free( buff -> array );
	free( buff -> tracker );
	free( buff );
}
