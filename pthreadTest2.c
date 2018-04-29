#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fileIO.h"

void* sharedRead( void* );
void* shoop( void* );
void* daWhoop( void* );
void* fileRead( void* );
int shooping = 1;
int reading = 0;
int end;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

typedef struct
{
	int* array;
	int length;
} Shared;

int main( int argc, char* argv[] )
{
	Shared shared;
	pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
	int i;
	FILE* file;

	shared.array = malloc( 10 * sizeof(int) );
	shared.length = 10;

	for ( i = 0; i < shared.length; i++ )
	{
		shared.array[i] = i;
	}

	pthread_create( &thread1, NULL, &sharedRead, &shared );
	pthread_create( &thread2, NULL, &sharedRead, &shared );

	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );

	file = openFile( argv[1] );

	pthread_create( &thread1, NULL, &fileRead, file );
	pthread_create( &thread2, NULL, &fileRead, file );
	pthread_create( &thread3, NULL, &fileRead, file );
	pthread_create( &thread4, NULL, &fileRead, file );
	pthread_create( &thread5, NULL, &fileRead, file );
	pthread_create( &thread6, NULL, &fileRead, file );
	
	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );
	pthread_join( thread3, NULL );
	pthread_join( thread4, NULL );
	pthread_join( thread5, NULL );
	pthread_join( thread6, NULL );
	
	/*
	pthread_create( &thread1, NULL, &shoop, NULL );
	pthread_create( &thread2, NULL, &daWhoop, NULL );

	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );
	*/
	return 0;
}

void* sharedRead( void* in )
{
	int i;
	Shared* shared = (Shared*)in;

	for ( i = 0; i < shared -> length; i++ )
	{
		printf( "Loop %d: %d\n", i, shared -> array[i] );
	}

	return in;
}

void* shoop( void* in )
{
	int i;

	for ( i = 0; i < 1000; i++ )
	{
		pthread_mutex_lock( &mutex1 );
		if ( !shooping )
		{
			pthread_cond_wait( &cond1, &mutex1 );
		}
		printf( "Shoop acquired lock : Shoop %d\n", i );
		shooping = 0;
		pthread_cond_signal( &cond1 );
		pthread_mutex_unlock( &mutex1 );
	}

	return 0;
}

void* daWhoop( void* in )
{
	int i;

	for ( i = 0; i < 1000; i++ )
	{
		pthread_mutex_lock( &mutex1 );
		if ( shooping )
		{
			pthread_cond_wait( &cond1, &mutex1 );
		}
		printf( "Da Whoop acquire lock : Da Whoop %d\n", i );
		shooping = 1;
		pthread_cond_signal( &cond1 );
		pthread_mutex_unlock( &mutex1 );
	}

	return 0;
}

void* fileRead( void* in )
{
	FILE* file = (FILE*)in;
	char word[10];
	int value;
	int pid;
	
	pid = pthread_self();

	do
	{
		pthread_mutex_lock( &mutex1 );
		if ( reading )
		{
			pthread_cond_wait( &cond1, &mutex1 );
		}
		reading = 1;
		end = fscanf( file, "%s", word );
		value = atoi( word );
		printf( "Thread %d read %d\n", pid, value );
		reading = 0;
		pthread_cond_signal( &cond1 );
		pthread_mutex_unlock( &mutex1 );
		sleep(1);
	} while ( end != EOF );

	return 0;
}
