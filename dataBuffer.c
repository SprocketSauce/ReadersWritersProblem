#include <stdlib.h>
#include "dataBuffer.h"

DataBuffer* createBuffer( int length, int readers )
{
	int i;
	DataBuffer* buff;
	
	buff = malloc( sizeof(DataBuffer) );
	buff -> array = malloc( length * sizeof(int) );
	buff -> tracker = malloc( length * sizeof(int) );
	buff -> length = length;
	buff -> index = 0;
	buff -> readers = readers;

	/* Buffer cells that have not been written to have a tracker
	 * value of -1 */
	for ( i = 0; i < length; i++ )
	{
		buff -> tracker[i] = -1;
	}

	return buff;
}

void writeTo( DataBuffer* buff, int value )
{
	buff -> tracker[index] = 0;
	buff -> array[index] = value;
	index++;
}

int isFull( DataBuffer* buff )
{
	return tracker[index] == readers;
}

void freeBuffer( DataBuffer* buff )
{
	free( buff -> array );
	free( buff -> tracker );
	free( buff );
}
