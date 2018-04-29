#include <stdlib.h>
#include <stdio.h>
#include "fileIO.h"

FILE* openFile( char* filename )
{
	FILE* file;

	file = fopen( filename, "r" );
	if ( file == NULL )
	{
		perror( "Error opening shared data" );
	}

	return file;
}
