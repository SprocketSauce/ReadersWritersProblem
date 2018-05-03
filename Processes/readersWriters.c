#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
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
	ReaderInput readIn;
	WriterInput writeIn;
	DataBuffer *buff;
	RWSemaphores *sem;
	Flags *flags;
	pthread_t *rthreads, *wthreads;
	int i, t1, t2, pid, readers, writers;
	int buffSM, semSM, flagSM, inFileSM, outFileSM;
	FILE *inFile, *outFile;

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

	inFile = openFile( argv[1] );
	outFile = fopen( "sim_out", "w" );

	readers = atoi( argv[2] );
	writers = atoi( argv[3] );
	t1 = atoi( argv[4] );
	t2 = atoi( argv[5] );

	if ( inFile == NULL || outFile == NULL )
	{
		return 1;
	}

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
	buffSM = shm_open( "buffer", O_CREAT | O_RDWR, 0666 );
	flagSM = shm_open( "flags", O_CREAT | O_RDWR, 0666 );
	inFileSM = shm_open( "input file", O_CREAT | O_RDWR, 0666 );
	outFileSM = shm_open( "output file", O_CREAT | O_RDWR, 0666 );

	/* Truncate shared memory segments */
	ftruncate( semSM, sizeof( RWSemaphores );
	ftruncate( buffSM, sizeof( DataBuffer );
	ftruncate( flagSM, sizeof( Flags );
	ftruncate( inFileSM, sizeof( FILE );
	ftruncate( outFileSM, sizeof( FILE );

	/* Map shared memory segments */
	sem = (RWSemaphores*)mmap( NULL, sizeof( RWSemaphores ), PROT_READ | PROT_WRITE,
		MAP_SHARED, semSM, 0 );
	buff = (DataBuffer*)mmap( NULL, sizeof( DataBuffer ), PROT_READ | PROT_WRITE,
		MAP_SHARED, buffSM, 0 );
	flags = (Flags*)mmap( NULL, sizeof( Flags ), PROT_READ | PROT_WRITE, MAP_SHARED,
		flagSM, 0 );
	inFile = (FILE*)mmap( NULL, sizeof( FILE ), PROT_READ | PROT_WRITE, MAP_SHARED,
		inFileSM, 0 );
	outFile = (FILE*)mmap( NULL, sizeof( FILE ), PROT_READ | PROT_WRITE, MAP_SHARED,
		outFileSM, 0 );
	

	/* Initialise semaphores */
	sem_init( sem -> mutex, 1 );
	sem_init( sem -> outMutex, 1 );
	sem_init( sem -> cond, 0 );

	/* Initialise data buffer */
	buff = createBuffer( BUFF_LENGTH, readers );

	/* Initialise flags */
	flags -> sdLength = 0;
	flags -> eof = 0;
	flags -> reading = 0;
	
	/* Create writers */
	i = 0;
	while ( i < writers && pid != 0 )
	{
		pid = fork();
	}

	/* Child processes execute writer */
	if ( pid == 0 )
	{
		/* WIP: EXECUTE WRITER PROCESS */
		_exit(0);
	}

	/* Create readers */
	i = 0;
	while ( i < readers && pid != 0 )
	{
		pid = fork();
	}

	/* Child processes execute reader */
	if ( pid == 0 )
	{
		/* WIP: EXECUTE READER PROCESS */
		_exit(0);			
	}
		
	/* Close files */
	fclose( inFile );
	fclose( outFile );

	printf( "%d items successfully read, results printed to sim_out\n", sdLength );

	pthread_mutex_destroy( &mutex );
	pthread_mutex_destroy( &outMutex );
	pthread_cond_destroy( &cond );

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
			pthread_cond_wait( &cond, &mutex );
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

		pthread_cond_signal( &cond );
		pthread_mutex_unlock( &mutex );
		
		sleep( in -> waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Writer %d has finished writing %d pieces of data to the data_buffer\n", pid, count );
	printToSimOut( in -> outFile, message );

	pthread_exit( 0 );
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

void printToSimOut( FILE* file, char message[100] )
{
	pthread_mutex_lock( &outMutex );
	
	fprintf( file, message );

	pthread_mutex_unlock( &outMutex );
}
