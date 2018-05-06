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

#include <stdlib.h>

/* ===== STRUCTS ===== */

/* STRUCT: DataBuffer
 * Stores an array of values and a tracker array that stores the number of
 * times each entry has been read. Has a single index, indicating the next
 * cell to be written to, and the number of readers, so that the buffer can
 * know when a cell is safe to overwrite.
 */
typedef struct
{
	int* array;
	int* tracker;
	int length;
	int index;
	int readers;
} DataBuffer;

/* ===== FUNCTION DECLARATIONS ===== */

DataBuffer* createBuffer( int, int );
void writeTo( DataBuffer*, int );
int isFull( DataBuffer* );
void freeBuffer( DataBuffer* );
