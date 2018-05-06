/* =============================================================================
 * FILE:     dataBuffer.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: dataBuffer.h
 *
 * PURPOSE:  Contains functionality for maintaining a shared data buffer for use
 *           with POSIX threads
 * =============================================================================
 */

#include "dataBuffer.h"

/*	============================================================================
 *	FUNCTION: createBuffer
 *	IMPORTS:  length:  The desired length of the buffer
 *            readers: The number of reader threads reading the buffer
 *  EXPORTS:  A pointer to the newly created buffer
 *
 *	PURPOSE:
 *	Allocates memory for a data buffer, initialises all of its values, and sets
 *  every cell in the tracker to -1.
 *	============================================================================
 */
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

/*	============================================================================
 *	FUNCTION: writeTo
 *	IMPORTS:  buff:  A pointer to the buffer to write to
 *            value: The value to write to the buffer
 *
 *	PURPOSE:
 *	Writes a value to an input data buffer. Sets the tracker on the newly
 *  written cell to 0.
 *	============================================================================
 */
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

/*	============================================================================
 *	FUNCTION: isFull
 *	IMPORTS:  buff: A pointer to the data buffer in question
 *  EXPORTS:  1 if the buffer is full, 0 if not
 *
 *	PURPOSE:
 *	Evaluates whether or not a cell can be safely written to. A cell can be
 *  written to if the cell has not yet been written to (tracker = -1) or if the
 *  cell has already been read by all readers (tracker = readers).
 *	============================================================================
 */
int isFull( DataBuffer* buff )
{
	return buff -> tracker[buff -> index] != -1 && buff -> tracker[buff -> index] != buff -> readers;
}

/*	============================================================================
 *	FUNCTION: freeBuffer
 *	IMPORTS:  buff: A pointer to the buffer to be freed
 *
 *	PURPOSE:
 *	Frees allocated memory associated with a buffer.
 *	============================================================================
 */
void freeBuffer( DataBuffer* buff )
{
	free( buff -> array );
	free( buff -> tracker );
	free( buff );
}
