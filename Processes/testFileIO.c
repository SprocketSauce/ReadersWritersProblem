#include <stdio.h>
#include "fileIO.h"

int main()
{
	int length, *array, i;
	
	array = readFile( "shared_data", &length );

	printf( "Length = %d\n Array: [ ", length );

	for ( i = 0; i < length; i++ )
	{
		printf( "%d ", array[i] );
	}

	printf( "]\n" );

	return 0;
}
