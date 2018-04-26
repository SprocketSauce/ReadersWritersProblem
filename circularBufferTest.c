#include <stdlib.h>
#include <stdio.h>
#include "circularBuffer.h"

int main()
{
	CircularBuffer* buff = createBuffer(5);

	if ( buff -> start != 0 || buff -> end != 0 )
	{
		printf( "ERROR1 " );
	}
	if ( !isEmpty( buff ) )
	{
		printf( "ERROR2 " );
	}
	if ( isFull( buff ) )
	{
		printf( "ERROR10 " );
	}

	if ( !writeTo( buff, 0 ) )
	{
		printf( "ERROR3 " );
	}
	if ( !writeTo( buff, 1 ) )
	{
		printf( "ERROR4 " );
	}
	if ( !writeTo( buff, 2 ) )
	{
		printf( "ERROR5 " );
	}
	if ( !writeTo( buff, 3 ) )
	{
		printf( "ERROR6 " );
	}
	if ( isEmpty( buff ) )
	{
		printf( "ERROR8 " );
	}
	if ( isFull( buff ) )
	{
		printf( "ERROR9 " );
	}
	if ( !writeTo( buff, 4 ) )
	{
		printf( "ERROR7 " );
	}
	if ( isEmpty( buff ) )
	{
		printf( "ERROR8 " );
	}
	if ( !isFull( buff ) )
	{
		printf( "ERROR9 " );
	}

	printf( "%d ", readFrom( buff ) );
	writeTo( buff, 5 );
	printf( "%d ", readFrom( buff ) );
	printf( "%d ", readFrom( buff ) );
	printf( "%d ", readFrom( buff ) );
	printf( "%d ", readFrom( buff ) );
	printf( "%d ", readFrom( buff ) );

	return 0;
}
