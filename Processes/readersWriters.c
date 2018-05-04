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
#include "readersWriters.h"

#define BUFF_LENGTH 20

/* ===== GLOBAL VARIABLES ===== */
int sdLength = 0; /* Length of the shared data file */
int eof = 0; /* Tracks whether then end of the file has been reached */
int reading = 0; /* Number of readers reading */
int readers = 0; /* Total number of readers */

int main( int argc, char* argv[] )
{
	DataBuffer *buff;
	RWSemaphores *sem;
	Flags *flags;
	int i, t1, t2, pid, readers, writers, *sharedData, sdLength;
	int semSM, flagSM;
	FILE* file;

	file = fopen( "sim_out", "w" );
	fclose( file );

	if ( argc != 6 )
	{
		printf( "ERROR: Invalid number of input arguments.\n"\
		"Correct usage: ./readersWriters [filename] [r] [w] [t1] [t2]\n"\
		"filename = Name of file to be read\n"\
		"r = Number of reader threads\n"\
		"w = Number of writer threads\n"\
		"t1 = Sleep time of reader threads\n"\
		"t2 = Sleep time of writer threads\n" );
		
		return 1;
	}

	readers = atoi( argv[2] );
	writers = atoi( argv[3] );
	t1 = atoi( argv[4] );
	t2 = atoi( argv[5] );

	if ( readers < 1 || writers < 1 )
	{
		printf( "ERROR: Reader and writer numbers must be positive integers" );
		return 1;
	}

	if ( t1 < 0 || t2 < 0 )
	{
		printf( "ERROR: Wait time must be a nonnegative integer" );
		return 1;
	}

	reading = 0;

	/* Create shared memory segments */
	semSM = shm_open( "semaphores", O_CREAT | O_RDWR, 0666 );
	flagSM = shm_open( "flags", O_CREAT | O_RDWR, 0666 );

	/* Truncate shared memory segments */
	ftruncate( semSM, sizeof( RWSemaphores ) );
	ftruncate( flagSM, sizeof( Flags ) );

	/* Map shared memory segments */
	sem = (RWSemaphores*)mmap( NULL, sizeof( RWSemaphores ), PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS, semSM, 0 );
	flags = (Flags*)mmap( NULL, sizeof( Flags ), PROT_READ | PROT_WRITE, MAP_SHARED |
		MAP_ANONYMOUS, flagSM, 0 );
	
	/* Initialise semaphores */
	sem_init( &sem -> mutex, 1, 1 );
	sem_init( &sem -> outMutex, 1, 1 );
	sem_init( &sem -> cond, 1, 0 );
	sem_init( &sem -> wrt, 1, 1 );
	sem_init( &sem -> done, 1, 0 );

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH, readers );
		
	/* Initialise flags */
	flags -> wIndex = 0;
	flags -> reading = 0;
	flags -> writing = 0;
	flags -> finished = 0;
	
	/* Read shared data from file */
	sharedData = readFile( argv[1], &sdLength );

	/* Create writers */
	i = 0;
	pid = getpid();
	while ( i < writers && pid != 0 )
	{
		i++;
		pid = fork();
		if ( pid == 0 )
		{
			writer( sem, buff, flags, sdLength, t2, sharedData );
		}
	}

	/* Create readers */
	i = 0;
	while ( i < readers && pid != 0 )
	{
		i++;
		pid = fork();
		if ( pid == 0 )
		{
			reader( sem, buff, flags, sdLength, readers, t1 );
		}
	}

	/* Finish child processes */
	if ( pid == 0 )
	{
		markFinished( sem, flags, readers + writers );
		_exit(0);
	}

	/* Wait for child processes to finish */
	sem_wait( &sem -> done );
	
	printf( "%d items successfully read, results printed to sim_out\n", sdLength );

	/* Destroy semaphores */
	sem_destroy( &sem -> mutex );
	sem_destroy( &sem -> outMutex );
	sem_destroy( &sem -> wrt );
	sem_destroy( &sem -> cond );
	sem_destroy( &sem -> done );

	/* Free shared memory */
	free( sharedData );
	freeBuffer( buff );
	close( semSM );
	close( flagSM );
	return 0;
}

void writer( RWSemaphores* sem, DataBuffer* buffer, Flags* flags, int sdLength, int waitTime, int* sharedData )
{
	int pid, value, count;
	char message[100];
	
	pid = getpid();
	count = 0;

	while ( flags -> wIndex < sdLength )
	{
		/* Waits for writer critical section to be free */
		sem_wait( &sem -> wrt );/*
		if ( flags -> reading >= 1 )
		{
			printf( "WRITER %d WAITING FOR COND\n", pid );
			sem_wait( &sem -> cond );
		}*/
		flags -> writing = 1;
	
		/* If the buffer is not full, read from shared data and write to the buffer */
		if ( !isFull( buffer ) && flags -> wIndex < sdLength )
		{
			value = sharedData[flags -> wIndex];
			writeTo( buffer, value );
			count++;
			flags -> wIndex++;
		}

		/* Frees the writer critical section */
		flags -> writing = 0;
		sem_post( &sem -> wrt );
		
		/* Performs busy wait */
		sleep( waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Writer %d has finished writing %d pieces of data to the data_buffer\n", pid, count );
	printToSimOut( sem, message );
}

void reader( RWSemaphores* sem, DataBuffer* buffer, Flags* flags, int sdLength, int readers, int waitTime )
{
	int value, pid, count, index, success, total;
	char message[100];
	
	pid = getpid();
	count = 0;
	index = 0;
	total = 0;
	
	while ( count < sdLength )
	{
		if ( count != flags -> wIndex )
		{
			/* Wait for reader critical section to be free */
			sem_wait( &sem -> mutex );

			/* Increment count of currently active readers */
			flags -> reading++;

			/* If this is the first reader and there is an active writer, wait for the 
			 * writer to finish */
			if ( flags -> reading == 1 /*&& flags -> writing == 1*/ )
			{
				sem_wait( &sem -> wrt );
			}
		
			/* Free reader critical section */
			sem_post( &sem -> mutex );

			/* Reads from the current cell of the data buffer, provided it has already been
			 * written to and has not already been read */
			success = 0;
			if ( buffer -> tracker[index] != -1 && buffer -> tracker[index]	!= readers )
			{
				value = buffer -> array[index];
				total += value;
				count++;
				index++;
				if ( index == BUFF_LENGTH )
				{
					index = 0;
				}
				success = 1;
			}
			else
			{
				printf( "READ FAILED\n" );
			}

			/* Wait for reader critical section to be free */
			sem_wait( &sem -> mutex );

			/* Increment tracker for previously read cell */
			if ( index != 0 && success )
			{
				buffer -> tracker[index - 1]++;
			}
			else if ( success )
			{
				buffer -> tracker[BUFF_LENGTH - 1]++;
			}

			/* Decrement count of currently active readers, signal writers if 0 */
			flags -> reading--;
			if ( flags -> reading == 0 )
			{
				sem_post( &sem -> wrt );
			}

			/* Free reader critical section */
			sem_post( &sem -> mutex );
	
			/* Performs busy wait */
			sleep( waitTime );
		}
	}

	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Reader %d has finished reading %d pieces of data from the data_buffer (total %d)\n", pid, count, total );
	printToSimOut( sem, message );
}

void printToSimOut( RWSemaphores* sem, char message[100] )
{
	FILE* file;
	
	sem_wait( &sem -> outMutex );
	
	file = fopen( "sim_out", "a" );
	fprintf( file, message );
	fclose( file );

	sem_post( &sem -> outMutex );
}

void markFinished( RWSemaphores* sem, Flags* flags, int processes )
{
	sem_wait( &sem -> outMutex );
	
	/* Increment number of finished processes */
	flags -> finished++;

	/* If all processes are finished, allow parent process to continue */
	if ( flags -> finished == processes )
	{
		sem_post( &sem -> done );
	}

	sem_post( &sem -> outMutex );
}
