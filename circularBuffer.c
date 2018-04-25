#include <stdlib.h>
#include "circularBuffer.h"

CircularBuffer* createBuffer( int inLength )
{
	CircularBuffer* buff;

	buff = malloc( sizeof(CircularBuffer) );
	buff -> array = malloc( inLength * sizeof(int) );
	buff -> length = inLength;
	buff -> start = 0;
	buff -> end = 0;

	return buff;
}

int writeTo( CircularBuffer* buff, int value )
{
	int worked = 1;
	
	if ( !isFull( buff ) )
	{
		buff -> array[buff -> end] = value;
		
		if ( buff -> end < buff -> length - 1 )
		{
			buff -> end = buff -> end + 1;
		}
		else
		{
			buff -> end = 0;
		}
	}
	else
	{
		worked = 0;
	}

	return worked;
}

int readFrom( CircularBuffer* buff )
{
	int value = 0;

	if ( !isEmpty( buff ) )
	{
		value = buff -> array[buff -> start];

		if ( buff -> start < buff -> length - 1 )
		{
			buff -> start = buff -> start + 1;
		}
		else
		{
			buff -> start = 0;
		}
	}

	return value;
}

int isFull( CircularBuffer* buff )
{
	int full = 0;

	if ( buff -> start == 0 )
	{
		full = buff -> end == buff -> length;
	}
	else
	{
		full = buff -> end == buff -> start - 1;
	}

	return full;
}

int isEmpty( CircularBuffer* buff )
{
	return buff -> start == buff -> end;
}

void freeBuffer( CircularBuffer* buff )
{
	free( buff -> array );
	free( buff );
}
