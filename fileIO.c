#include <stdlib.h>
#include <stdio.h>
#include "fileIO.h"

FILE* openFile( char* filename )
{
	FILE* file;
	char message[100];

	file = fopen( filename, "r" );
	if ( file == NULL )
	{
		sprintf( message, "Error opening %s", filename );
		perror( message );
	}

	return file;
}
