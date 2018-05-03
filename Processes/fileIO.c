#include <stdlib.h>
#include <stdio.h>
#include "fileIO.h"

int* readFile( char* filename, int* length )
{
	FILE *file;
	int *array, eof, i;
	char message[100];

	/* Open file */
	file = fopen( filename, "r" );
	if ( file == NULL )
	{
		sprintf( message, "Error opening %s", filename );
		perror( message );
	}

	/* Find length of file */
	eof = 0;
	*length = 0;
	while( eof != EOF )
	{
		eof = fscanf( file, "%d ", &i );
		if ( eof != EOF )
		{
			(*length)++;
		}
	}

	/* Reset pointer to start of file */
	rewind( file );

	/* Allocate array based on file length */
	array = malloc( sizeof(int) * *length );

	/* Read file into array */
	for ( i = 0; i < *length; i++ )
	{
		fscanf( file, "%d ", &array[i] );
	}

	/* Close file */
	fclose( file );

	return array;
}
