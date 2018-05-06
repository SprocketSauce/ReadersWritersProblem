/* =============================================================================
 * FILE:     readersWriters.h
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: fileIO.h, fileIO.c, dataBuffer.h, dataBuffer.c, readersWriters.h
 *
 * PURPOSE: Header file for readersWriters.c
 * =============================================================================
 */

/* ===== IMPORTS ===== */

#define _GNU_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include "fileIO.h"
#include "dataBuffer.h"

/* ===== DEFINITIONS ===== */

#define BUFF_LENGTH 20

/* ===== STRUCTS ===== */

/* STRUCT: RWSempahores
 * Stores POSIX sempahores used for process synchronisation.
 */
typedef struct
{
	sem_t mutex;
	sem_t wrt;
	sem_t outMutex;
	sem_t cond;
	sem_t done;
} RWSemaphores;

/* STRUCT: Flags
 * Stores counters and flags that must be shared between processes. Stores the
 * index of the next cell of the shared data array to be copied by the writers,
 * the number of readers currently reading, whether or not there is an active
 * writer, and the number of finished processes.
 */
typedef struct
{
	int wIndex;
	int reading;
	int finished;
} Flags;

/* ===== FUNCTION DECLARATION ===== */

void reader( RWSemaphores*, DataBuffer*, Flags*, int, int, int );
void writer( RWSemaphores*, DataBuffer*, Flags*, int, int, int* );
void printToSimOut( RWSemaphores*, char[100] );
void markFinished( RWSemaphores*, Flags*, int );
