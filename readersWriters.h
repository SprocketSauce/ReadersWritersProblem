/* =============================================================================
 * FILE:     readersWriters.h
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: fileIO.h, fileIO.c, dataBuffer.h, dataBuffer.c
 *
 * PURPOSE:  Header file for readersWriters.c
 * =============================================================================
 */
/* ===== IMPORTS ===== */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fileIO.h"
#include "dataBuffer.h"

/* ===== DEFINITIONS ===== */

#define BUFF_LENGTH 20

/* ===== STRUCTS ===== */

/* STRUCT: WriterInput
 * Input struct for writer threads. Stores a pointer to the shared data file,
 * sim_out, the writer's sleep time and a pointer to the data buffer.
 */
typedef struct
{
	FILE* inFile;
	FILE* outFile;
	int waitTime;
	DataBuffer* buffer;
} WriterInput;

/* STRUCT: ReaderInput
 * Input struct for reader threads. Stores a pointer to sim_out, the reader's
 * sleep time and a pointer to the data buffer.
 */
typedef struct
{
	FILE* outFile;
	int waitTime;
	DataBuffer* buffer;
} ReaderInput;

/* ===== FUNCTION DECLARATIONS ===== */

void* reader( void* );
void* writer( void* );
void printToSimOut( FILE*, char[100] );
