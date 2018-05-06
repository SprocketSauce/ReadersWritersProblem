/* =============================================================================
 * FILE:     readersWriters.c
 * AUTHOR:   Jack McNair 18927430
 * UNIT:     COMP2006 - Operating Systems
 * LAST MOD: 06/05/2018
 * REQUIRES: fileIO.h, fileIO.c, dataBuffer.h, dataBuffer.c, readersWriters.h
 *
 * PURPOSE:  Employs multiple processes and POSIX semaphores to write values 
 *           from a file to a data buffer, then read those values. Prints 
 *           results to a file called sim_out.
 * =============================================================================
 */

#include "readersWriters.h"

/* ===== GLOBAL VARIABLES ===== */

int main( int argc, char* argv[] )
{
	DataBuffer *buff;
	RWSemaphores *sem;
	Flags *flags;
	int i, t1, t2, pid, readers, writers, *sharedData, sdLength;
	int semSM, flagSM;
	FILE* file;

	/* Clears output file */
	file = fopen( "sim_out", "w" );
	fclose( file );

	/* Prints an error and stops execution if an invalid number of input
	 * arguments were provided
	 */
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

	/* Initialises number of readers, writers and wait times */
	readers = atoi( argv[2] );
	writers = atoi( argv[3] );
	t1 = atoi( argv[4] );
	t2 = atoi( argv[5] );

	/* Stops execution if inputs are invalid */
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

/* =============================================================================
 * FUNCTION: writer
 * IMPORTS:  sem:        Pointer to a struct containing POSIX semaphores
 *           buffer:     Pointer to a struct containing the shared data buffer
 *           flags:      Pointer to a struct containing counters and flags
 *           sdLength:   Number of integers in the shared data file
 *           waitTime:   Sleep time for writers
 *           sharedData: An array of integers read in from a file
 *
 * PURPOSE:
 * Writes a value from the shared data array to the data buffer. Stops when
 * every item in the array has been read by a reader. Gives preference to
 * readers - if there are any readers reading from the data buffer, writers
 * will wait for them to finish. Only one writer can write at a time. Once all
 * items in the array have been read, writers print the number of entries they
 * have written to sim_out.
 * =============================================================================
 */
void writer( RWSemaphores* sem, DataBuffer* buffer, Flags* flags, int sdLength, int waitTime, int* sharedData )
{
	int pid, value, count;
	char message[100];
	
	pid = getpid();
	count = 0;

	while ( flags -> wIndex < sdLength )
	{
		/* Waits for writer critical section to be free */
		sem_wait( &sem -> wrt );

		/* If the buffer is not full, read from shared data and write to the buffer */
		if ( !isFull( buffer ) && flags -> wIndex < sdLength )
		{
			value = sharedData[flags -> wIndex];
			writeTo( buffer, value );
			count++;
			flags -> wIndex++;
		}

		/* Frees the writer critical section */
		sem_post( &sem -> wrt );
		
		/* Performs busy wait */
		sleep( waitTime );
	}
	
	/* Prints number of buffer writes to sim_out */
	sprintf( message, "Writer %d has finished writing %d pieces of data to the data_buffer\n", pid, count );
	printToSimOut( sem, message );
}

/* =============================================================================
 * FUNCTION: reader
 * IMPORTS:  sem:        Pointer to a struct containing POSIX semaphores
 *           buffer:     Pointer to a struct containing the shared data buffer
 *           flags:      Pointer to a struct containing counters and flags
 *           sdLength:   Number of integers in the shared data file
 *           readers:    Total number of reader processes
 *           waitTime:   Sleep time for writers
 *
 * PURPOSE:
 * Reads from the data buffer. Increments the tracker corresponding to the read
 * cell so that wirters can tell if it is safe to overwrite a cell. The reading
 * section is braced by a pair of critical sections - this means that multiple
 * writers can be reading from the buffer simultaneously, but only a single
 * reader can be altering relevant values at once. Upon reaching the end of a
 * loop, if there are no other active readers, the last writer will post to the
 * writer semaphore. Upon reading every integer from the shared data array, the
 * writer exits its loop and prints its total number of reads and sum of read
 * values to sim_out
 * =============================================================================
 */
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
			if ( flags -> reading == 1 )
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

/* =============================================================================
 * FUNCTION: printToSimOut
 * IMPORTS:  sem:     Pointer to a struct containing POSIX semaphores
 *           message: A string to be printed to the output file
 * 
 * PURPOSE:
 * Appends a message to the end of a file called "sim_out". Designed to be used
 * by multiple processes, so will wait for a semaphore to be signalled before
 * opening the file, and will signal that semaphore after closing it.
 * =============================================================================
 */
void printToSimOut( RWSemaphores* sem, char message[100] )
{
	FILE* file;
	
	sem_wait( &sem -> outMutex );
	
	file = fopen( "sim_out", "a" );
	fprintf( file, message );
	fclose( file );

	sem_post( &sem -> outMutex );
}

/* =============================================================================
 * FUNCTION: markFinished
 * IMPORTS:  sem:       A pointer to a struct containing POSIX semaphores
 *           flags:     Pointer to a struct containing counters and flags
 *           processes: The total number of child processes
 * 
 * PURPOSE:
 * Increments a tally of the number of finished processes. If all processes have
 * completed, signals a semaphore to allow the parent process to continue. Uses
 * the outMutex semaphore to prevent race conditions.
 * =============================================================================
 */
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
