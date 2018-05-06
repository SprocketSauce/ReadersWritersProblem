/* =============================================================================
 * FILE:     dataBuffer.h
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: N/A
 *
 * PURPOSE:  Header file for dataBuffer.c
 * =============================================================================
 */

/* ===== IMPORTS ===== */

#define _GNU_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* ===== STRUCTS ===== */

/* STRUCT: DataBuffer
 * Stores an array of values and a tracker array that stores the number of times
 * each array entry has been read. Has a single index, indicating the next cell
 * to be written to, and the number of readers, so that the buffer can know when
 * a cell is safe to overwrite. The buffer also stores its own file descriptor,
 * as well as the file descriptors of its two arrays.
 */
typedef struct
{
	int* array;
	int* tracker;
	int length;
	int index;
	int readers;
	int buffSM;
	int arraySM;
	int trackSM;
} DataBuffer;

/* ===== FUNCTION DECLARATIONS ===== */

DataBuffer* createBuffer( int, int );
void writeTo( DataBuffer*, int );
int isFull( DataBuffer* );
void freeBuffer( DataBuffer* );
