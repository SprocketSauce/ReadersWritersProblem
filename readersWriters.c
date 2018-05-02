#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fileIO.h"
#include "dataBuffer.h"
#include "readersWriters.h"

#define BUFF_LENGTH 20

/* ===== GLOBAL VARIABLES ===== */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t rcond = PTHREAD_COND_INITIALIZER;
int sdLength = 0; /* Length of the shared data file */
int eof = 0; /* Tracks whether then end of the file has been reached */
int rcrit = 0; /* BOOL: is anything in the reader critical section? */
int reading = 0; /* Number of readers reading */
int writing = 0; /* BOOL: is anything writing? */
int readers = 0; /* Total number of readers */
int writers = 0; /* Total number of writers */

int main( int argc, char* argv[] )
{
	ReaderInput readIn;
	WriterInput writeIn;
	DataBuffer *buff;
	pthread_t *rthreads, *wthreads;
	int i;
	FILE *inFile, *outFile;

	inFile = openFile( argv[1] );
	outFile = fopen( "sim_out", "w" );

	readers = atoi( argv[2] );
	writers = atoi( argv[3] );

	rthreads = malloc( readers * sizeof(pthread_t) );
	wthreads = malloc( writers * sizeof(pthread_t) );

	reading = 0;
	writing = 0;

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH, readers );
	
	/* Create writer input struct */
	writeIn.inFile = inFile;
	writeIn.outFile = outFile;
	writeIn.waitTime = atoi( argv[5] );
	writeIn.buffer = buff;

	/* Create reader input struct */
	readIn.outFile = outFile;
	readIn.waitTime = atoi( argv[4] );
	readIn.buffer = buff;
	
	/* Create writers */
	for ( i = 0; i < writers; i++ )
	{
		pthread_create( &wthreads[i], NULL, &writer, &writeIn);
	}

	/* Create readers */
	for ( i = 0; i < readers; i++ )
	{
		pthread_create( &rthreads[i], NULL, &reader, &readIn );
	}

	/* Join writers */
	for ( i = 0; i < writers; i++ )
	{
		pthread_join( wthreads[i], NULL );
	}

	/* Join readers */
	for ( i = 0; i < readers; i++ )
	{
		pthread_join( rthreads[i], NULL );
	}

	/* Close files */
	fclose( inFile );
	fclose( outFile );

	/* Free allocated memory */
	freeBuffer( buff );
	free( rthreads );
	free( wthreads );

	return 0;
}

void* writer( void* voidIn )
{
	int pid, value, count;
	WriterInput* in;
	char str[10], message[100];
	
	pid = pthread_self();
	in = (WriterInput*)voidIn;
	count = 0;

	while ( eof != EOF )
	{
		/* Waits for writer critical section to be free */
		pthread_mutex_lock( &mutex );
		if ( reading >= 1 )
		{
			pthread_cond_wait( &wcond, &mutex );
		}
	
		/* If the buffer is not full, read from shared data and write to the buffer */
		if ( !isFull( in -> buffer ) && eof != EOF )
		{
			eof = fscanf( in -> inFile, "%s", str );
			
			if ( eof != EOF )
			{
				value = atoi( str );
				writeTo( in -> buffer, value );
				count++;
				sdLength++;
			}
		}

		pthread_cond_signal( &wcond );
		pthread_mutex_unlock( &mutex );
		
		sleep( in -> waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Writer %d has finished writing %d pieces of data to the data_buffer\n", pid, count );
	printToSimOut( in -> outFile, message );

	return 0;
}

void* reader( void* voidIn )
{
	int value, pid, count, index, success, total;
	char message[100];
	ReaderInput* in;
	
	pid = pthread_self();
	in = (ReaderInput*)voidIn;
	count = 0;
	index = 0;
	total = 0;

	while ( eof != EOF || count != sdLength )
	{
		if ( count != sdLength )
		{
			/* Wait for reader critical section to be free */
			pthread_mutex_lock( &mutex );
			
			/* Increment count of currently active readers */
			reading++;

			/* Free reader critical section */
			pthread_mutex_unlock( &mutex );

			/* Reads from the current cell of the data buffer, provided it has already been
			 * written to and has not already been read */
			success = 0;
			if ( in -> buffer -> tracker[index] != -1 && in -> buffer -> tracker[index] 
				!= readers )
			{
				value = in -> buffer -> array[index];
				/*printf( "Reader %d read %d, count %d, sdlen %d\n", pid, value, count + 1, sdLength );*/
				total += value;
				count++;
				index++;
				if ( index == BUFF_LENGTH )
				{
					index = 0;
				}
				success = 1;
			}

			/* Wait for reader critical section to be free */
			pthread_mutex_lock( &mutex );

			/* Increment tracker for previously read cell */
			if ( index != 0 && success )
			{
				in -> buffer -> tracker[index - 1]++;
			}
			else if ( success )
			{
				in -> buffer -> tracker[BUFF_LENGTH - 1]++;
			}

			/* Decrement count of currently active readers, signal writers if 0 */
			reading--;
			if ( reading == 0 )
			{
				pthread_cond_signal( &wcond );
			}

			/* Free reader critical section */
			pthread_mutex_unlock( &mutex );
		}

		sleep( in -> waitTime );
	}

	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Reader %d has finished reading %d pieces of data from the data_buffer (total %d)\n", pid, count, total );
	printToSimOut( in -> outFile, message );

	return 0;
}

void printToSimOut( FILE* file, char message[100] )
{
	pthread_mutex_lock( &outMutex );
	
	fprintf( file, message );

	pthread_mutex_unlock( &outMutex );
}
