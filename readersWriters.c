/* =============================================================================
 * FILE:     readersWriters.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: fileIO.h, fileIO.c, dataBuffer.h, dataBuffer.c, readersWriters.h
 *
 * PURPOSE:  Implements POSIX multithreading in order to write values from a
 *           file to a data buffer, then read those values. Prints results
 *           to a file called sim_out.
 * =============================================================================
 */

#include "readersWriters.h"

/* ===== GLOBAL VARIABLES ===== */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int sdLength = 0; /* Length of the shared data file */
int eof = 0; /* Tracks whether then end of the file has been reached */
int reading = 0; /* Number of readers reading */
int readers = 0; /* Total number of readers */
int writers = 0; /* Total number of writers */

int main( int argc, char* argv[] )
{
	/* ===== LOCAL VARIABLES ===== */
	ReaderInput readIn;
	WriterInput writeIn;
	DataBuffer *buff;
	pthread_t *rthreads, *wthreads;
	int i, t1, t2;
	FILE *inFile, *outFile;

	/* Prints an error if there is an incorrect number of input arguments */
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

	/* Opens the input file */
	inFile = openFile( argv[1] );

	/* Opens the output file */
	outFile = fopen( "sim_out", "w" );

	/* Initialises number of readers, writers and wait times using input
	 * arguments */
	readers = atoi( argv[2] );
	writers = atoi( argv[3] );
	t1 = atoi( argv[4] );
	t2 = atoi( argv[5] );

	/* Exits if either file did not open correctly */
	if ( inFile == NULL || outFile == NULL )
	{
		return 1;
	}

	/* Exits if inputs are invalid */
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

	/* Allocates memory space for threads */
	rthreads = malloc( readers * sizeof(pthread_t) );
	wthreads = malloc( writers * sizeof(pthread_t) );

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH, readers );
	
	/* Create writer input struct */
	writeIn.inFile = inFile;
	writeIn.outFile = outFile;
	writeIn.waitTime = t2;
	writeIn.buffer = buff;

	/* Create reader input struct */
	readIn.outFile = outFile;
	readIn.waitTime = t1;
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

	printf( "%d items successfully read, results printed to sim_out\n", sdLength );

	/* Destroys mutexes and conditional */
	pthread_mutex_destroy( &mutex );
	pthread_mutex_destroy( &outMutex );
	pthread_cond_destroy( &cond );

	/* Free allocated memory */
	freeBuffer( buff );
	free( rthreads );
	free( wthreads );

	return 0;
}

/*	============================================================================
 *	FUNCTION: writer
 *	IMPORTS:  voidIn, a void pointer pointing to a WriterInput struct. This
 *	struct has 4 fields:
 *            inFile:   pointer to the input file
 *            outFile:  pointer to the output file
 *            waitTime: integer indicating sleep time in seconds
 *            buffer:   pointer to the data buffer, represented by a DataBuffer
 *            			struct.
 *
 *	PURPOSE:
 *	Writes a value from the shared data file to the data buffer. Sets eof to
 *	EOF if it hits the end of the file. Gives preference to readers - if there
 *	are any readers reading from the data buffer, writer will wait for them
 *	to finish. Only one writer can write at a time. Once a writer reaches the 
 *	end of the shared data file, all writers terminate and print the number of 
 *	entries they have written to sim_out.
 *  ============================================================================
 */
void* writer( void* voidIn )
{
	int pid, value, count;
	WriterInput* in;
	char str[10], message[100];
	
	pid = pthread_self();
	in = (WriterInput*)voidIn;
	count = 0;

	/* Continues to loop until the end of the input file is reached */
	while ( eof != EOF )
	{
		/* Waits for mutex to be free */
		pthread_mutex_lock( &mutex );
		
		/* If a reader is currently reading, wait for all readers to finish */
		if ( reading >= 1 )
		{
			pthread_cond_wait( &cond, &mutex );
		}
	
		/* If the buffer is not full, read from shared data and write to the buffer */
		if ( !isFull( in -> buffer ) && eof != EOF )
		{
			/* Assigns the next unread entry in the shared data file to str */
			eof = fscanf( in -> inFile, "%s", str );
			
			/* If the end of the file has not been reached, write to the data
			 * buffer */
			if ( eof != EOF )
			{
				value = atoi( str );
				writeTo( in -> buffer, value );
				count++;
				sdLength++;
			}
		}

		/* Signal writers waiting for readers, since multiple readers may be waiting
		 * simultaneously, but readers only send one signal when all are finished */
		pthread_cond_signal( &cond );

		/* Unlock mutex */
		pthread_mutex_unlock( &mutex );
		
		/* Sleep for predefined time span */
		sleep( in -> waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Writer %d has finished writing %d pieces of data to the data_buffer\n", 
		pid, count );
	printToSimOut( in -> outFile, message );

	/* Terminate the thread */
	pthread_exit( 0 );
}

/*	============================================================================
 *	FUNCTION: reader
 *	IMPORTS:  voidIn, a void pointer pointing to a ReaderInput struct. This
 *	struct has 3 fields:
 *            outFile:  pointer to the output file
 *            waitTime: integer indicating sleep time in seconds
 *            buffer:   pointer to the data buffer, represented by a DataBuffer
 *            			struct.
 *
 *	PURPOSE:
 *  Reads from the data buffer. Increments the tracker corresponding to the
 *  read cell so that writers can tell if it is safe to overwrite a cell. The
 *  reading section is braced by a pair of critical sections - this means that
 *  multiple readers can be reading from the buffer simultaneously, but only
 *  a single reader can be altering relevant values at once. Signals waiting
 *  writers at the end of a loop if there are no other readers currently
 *  reading. Upon reading every entry in the shared data file, the writer exits,
 *  printing its total number of reads and the sum of all read values to
 *  sim_out.
 *	============================================================================
 */
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

	/* Loops until every value from the input file has been read from the
	 * shared data buffer */
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
				pthread_cond_signal( &cond );
			}

			/* Free reader critical section */
			pthread_mutex_unlock( &mutex );
		}

		sleep( in -> waitTime );
	}

	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Reader %d has finished reading %d pieces of data from the data_buffer (total %d)\n", pid, count, total );
	printToSimOut( in -> outFile, message );

	pthread_exit( 0 );
}

/*	============================================================================
 *	FUNCTION: printToSimOut
 *	IMPORTS:  file:    a file pointer to the file to be written to.
 *            message: a string to be written to the file.
 *
 *	PURPOSE:
 *	Prints a message to a file. Before writing to the file, acquires a mutex
 *	lock. Releases the mutex after writing.
 *	============================================================================
 */
void printToSimOut( FILE* file, char message[100] )
{
	pthread_mutex_lock( &outMutex );
	
	fprintf( file, message );

	pthread_mutex_unlock( &outMutex );
}
