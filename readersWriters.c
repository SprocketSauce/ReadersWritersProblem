#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fileIO.h"
#include "dataBuffer.h"
#include "readersWriters.h"

#define BUFF_LENGTH 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t rcond = PTHREAD_COND_INITIALIZER;

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

	r = argv[2];
	w = argv[3];

	rthreads = malloc( r * sizeof(pthread_t) );
	wthreads = malloc( w * sizeof(pthread_t) );

	reading = 0;
	writing = 1;

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH );
	
	/* Create writer input struct */
	writeIn.inFile = inFile;
	writeIn.outFile = outFile;
	writeIn.waitTime = argv[5];
	writeIn.readers = r;
	writeIn.reading = &reading;
	writeIn.writing = &writing;
	writeIn.buffer = buff;

	/* Create reader input struct */
	readIn.outFile = outFile;
	readIn.waitTime = argv[4];
	readIn.reading = &reading;
	readIn.writing = &writing;
	readIn.buffer = buff;

	for ( i = 0; i < w; i++ )
	{
		pthread_create( &wthreads[i], NULL, &writer, &writeIn);
	}

	for ( i = 0; i < r; i++ )
	{
		pthread_create( &rthreads[i], NULL, &reader, &readIn );
	}
}

void* writer( void* in )
{
	int pid;
	
	pid = pthread_self();
	
}

void* reader( void* in )
{
}
