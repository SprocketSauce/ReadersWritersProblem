#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* sharedRead( int*, int );
void* shoop();
void* daWhoop();
int shooping = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int main()
{
	int shared[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	pthread_t thread1, thread2;

	pthread_create( &thread1, NULL, &shoop, NULL );
	pthread_create( &thread2, NULL, &daWhoop, NULL );

	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );

	return 0;
}

void* sharedRead( int* array, int length )
{
	int i;

	for ( i = 0; i < length; i++ )
	{
		printf( "Loop %d: %d\n", i, array[i] );
	}
}

void* shoop()
{
	int i;

	for ( i = 0; i < 1000; i++ )
	{
		printf( "Shoop locking mutex\n" );
		pthread_mutex_lock( &mutex1 );
		if ( !shooping )
		{
			printf( "Shoop waiting for lock\n" );
			pthread_cond_wait( &cond1, &mutex1 );
		}
		printf( "Shoop acquired lock : Shoop %d\n", i );
		shooping = 0;
		printf( "Shoop signaling condition\n" );
		pthread_cond_signal( &cond1 );
		printf( "Shoop unlocking mutex\n");
		pthread_mutex_unlock( &mutex1 );
	}
}

void* daWhoop()
{
	int i;

	for ( i = 0; i < 1000; i++ )
	{
		printf( "Da Whoop locking mutex\n" );
		pthread_mutex_lock( &mutex1 );
		if ( shooping )
		{
			printf( "Da Whoop waiting for lock\n" );
			pthread_cond_wait( &cond1, &mutex1 );
		}
		printf( "Da Whoop acquire lock : Da Whoop %d\n", i );
		shooping = 1;
		printf( "Da Whoop signalling condition\n" );
		pthread_cond_signal( &cond1 );
		printf( "Da Whoop unlocking mutex\n" );
		pthread_mutex_unlock( &mutex1 );
	}
}
