/* =============================================================================
 * FILE:     fileIO.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: fileIO.h
 *
 * PURPOSE:  Contains functionality for opening a read-only file
 * =============================================================================
 */

#include "fileIO.h"

/*	============================================================================
 *	FUNCTION: writer
 *	IMPORTS:  filename: The name of the file to be opened
 *  EXPORTS:  A pointer to the opened file, or NULL in the event of failure
 *
 *	PURPOSE:
 *  Opens a file in read-only mode, then returns a pointer to the file.
 *  ============================================================================
 */
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
