#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fileIO.h"
#include "dataBuffer.h"
#include "readersWriters.h"

#define BUFF_LENGTH 20

pthread_mutex_t wmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t rcond = PTHREAD_COND_INITIALIZER;
int sdLength = 0;
int eof = 0;
int rcrit = 0;

int main( int argc, char* argv[] )
{
	ReaderInput readIn;
	WriterInput writeIn;
	DataBuffer *buff;
	pthread_t *rthreads, *wthreads;
	int r, w, i, reading, writing;
	FILE *inFile, *outFile;

	inFile = openFile( argv[1] );
	outFile = fopen( "sim_out", "w" );

	r = atoi( argv[2] );
	w = atoi( argv[3] );

	rthreads = malloc( r * sizeof(pthread_t) );
	wthreads = malloc( w * sizeof(pthread_t) );

	reading = 0;
	writing = 0;

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH );
	
	/* Create writer input struct */
	writeIn.inFile = inFile;
	writeIn.outFile = outFile;
	writeIn.waitTime = atoi( argv[5] );
	writeIn.readers = r;
	writeIn.reading = &reading;
	writeIn.writing = &writing;
	writeIn.buffer = buff;

	/* Create reader input struct */
	readIn.outFile = outFile;
	readIn.waitTime = atoi( argv[4] );
	readIn.reading = &reading;
	readIn.writing = &writing;
	readIn.buffer = buff;
	
	/* Create writers */
	for ( i = 0; i < w; i++ )
	{
		pthread_create( &wthreads[i], NULL, &writer, &writeIn);
	}

	/* Create readers */
	for ( i = 0; i < r; i++ )
	{
		pthread_create( &rthreads[i], NULL, &reader, &readIn );
	}

	/* Join writers */
	for ( i = 0; i < w; i++ )
	{
		pthread_join( wthreads[i], NULL );
	}

	/* Join readers */
	for ( i = 0; i < r; i++ )
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
	int pid, value, counter;
	WriterInput* in;
	char str[10];
	
	pid = pthread_self();
	in = (WriterInput*)voidIn;
	counter = 0;

	while ( eof != EOF )
	{
		pthread_mutex_lock( &wmutex );
		if ( in -> reading >= 1 || in -> writing == 1 )
		{
			pthread_cond_wait( &wcond, &wmutex );
		}

		in -> writing = 1;
		
		/* If the buffer is not full, read from shared data and write to the buffer */
		if ( !isFull( in -> buffer ) && eof != EOF )
		{
			eof = fscanf( in -> inFile, "%s", str );
			value = atoi( str );
			writeTo( in -> buffer, value );
			counter++;
			sdlength++;
		}

		in -> writing = 0;
		pthread_cond_signal( &wcond );
		pthread_mutex_unlock( &wmutex );

		sleep( in -> waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	fprintf( in -> outFile, "Writer-%d has finished writing %d pieces of data to the data_buffer\n", pid, count );

	return 0;
}

void* reader( void* voidIn )
{
	int value, pid, counter, index; 
	ReaderInput* in;
	
	pid = pthread_self();
	in = (ReaderInput*)voidIn;
	counter = 0;
	index = 0;

	while ( eof != EOF || count = sdLength )
	{
		/* Wait for reader critical section to be free */
		pthread_mutex_lock( &rmutex );
		if ( rcrit == 1 )
		{
			pthread_cond_wait( &rmutex, &rcond );
		}

		/* Increment count of currently active readers */
		in -> reading = in -> reading++;

		/* If this is the only active reader and there is an active writer, wait for 
		 * writers to finish */
		pthread_mutex_lock( &wmutex );
		if ( in -> reading == 1 && in -> writing == 1 )
		{
			pthread_cond_wait( &wmutex, &wcond );
		}

		/* Reads from the current cell of the data buffer, provided it has already been
		 * written to. */
		if ( in -> buff -> tracker[index] != -1 )
		{
			value = in -> buff -> array[index];
			counter++;
			index++;
		}

		/* WIP, CONTINUE HERE */
	}

	fprintf( in -> outFile, "Reader %d successfully created\n", pid );
	return 0;
}
