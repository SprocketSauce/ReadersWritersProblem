/* =============================================================================
 * FILE:     fileIO.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/08/2018
 * REQUIRES: fileIO.h
 * 
 * PURPOSE:  Contains functionality for reading a file into a malloc'd array
 * =============================================================================
 */

#include "fileIO.h"

/* =============================================================================
 * FUNCTION: readFile
 * IMPORTS:  filename: The name of the file to be opened
 *           length:   A pointer to an integer that will store the length of the
 *                     array
 * EXPORTS:  A pointer to the created array
 *
 * PURPOSE:
 * Opens a file, reads integers from that file, allocates memory for an array
 * big enough to hold all these integers, and writes the integers to the array.
 * The number of integers read is saved to the memory address pointed to by
 * "length".
 * =============================================================================
 */
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
