/* =============================================================================
 * FILE:     dataBuffer.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/08/2018
 * REQUIRES: dataBuffer.h
 *
 * PURPOSE:  Contains functionality for maintaining a data buffer in shared
 * memory, able to be accessed by multiple processes.
 * =============================================================================
 */

#define _GNU_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dataBuffer.h"

/* =============================================================================
 * FUNCTION: createBuffer
 * IMPORTS:  length:  The desired length of the buffer
 *           readers: The number of processes reading the buffer
 * EXPORTS:  A pointer to the newly created buffer
 *
 * PURPOSE:
 * Opens and maps a shared memory segment for a data buffer. Initialises all its
 * values, opens shared memory segments for the buffer's array and tracker, and
 * initialises all of the tracker's cells to -1.
 * =============================================================================
 */
DataBuffer* createBuffer( int length, int readers )
{
	int i, fd;
	DataBuffer* buff;

	/* Create shared memory for the whole buffer */
	fd = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );
	ftruncate( fd, sizeof( DataBuffer ) );
	buff = (DataBuffer*)mmap( NULL, sizeof( DataBuffer ), PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, 0 );
	
	/* Initialises buffer values */
	buff -> buffSM = fd;
	buff -> length = length;
	buff -> index = 0;
	buff -> readers = readers;

	/* Create shared memory segments */
	buff -> arraySM = shm_open( "array", O_CREAT | O_RDWR, 0666 );
	buff -> trackSM = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );

	/* Truncate shared memory segments */
	ftruncate( buff -> arraySM, sizeof(int) * length );
	ftruncate( buff -> trackSM, 80 );

	/* Map shared memory segments */
	buff -> array = (int*)mmap( NULL, sizeof(int) * length, PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, buff -> arraySM, 0 );
	buff -> tracker = (int*)mmap( NULL, sizeof(int), PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, buff -> trackSM, 0 );

	/* Buffer cells that have not been written to have a tracker value of -1 */
	for ( i = 0; i < length; i++ )
	{
		buff -> tracker[i] = -1;
	}

	return buff;
}

/* =============================================================================
 * FUNCTION: writeTo
 * IMPORTS:  buff:  A pointer to the buffer to write to
 *           value: The value to write to the buffer
 *
 * PURPOSE:
 * Writes a value to an input data buffer. Sets the tracker on the newly
 * written cell to 0.
 * =============================================================================
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

/* =============================================================================
 * FUNCTION: isFull
 * IMPORTS:  buff:  A pointer to the data buffer in question
 * EXPORTS:  1 if the buffer is full, 0 if not
 *
 * PURPOSE:
 * Evaluates whether or not a cell can be safely written to. A cell can be
 * written to if the cell had not yet been written to (tracker = -1) or if the
 * cell has already been read by all readers (tracker = readers).
 * =============================================================================
 */
int isFull( DataBuffer* buff )
{
	return buff -> tracker[buff -> index] != -1 && buff -> tracker[buff -> index] != buff -> readers;
}

/* =============================================================================
 * FUNCTION: freeBuffer
 * IMPORTS:  buff:  A pointer to the data buffer to be freed
 *
 * PURPOSE:
 * Closes shared memory segments associated with a buffer.
 * =============================================================================
 */
void freeBuffer( DataBuffer* buff )
{
	close( buff -> arraySM );
	close( buff -> trackSM );
	close( buff -> buffSM );
}
