#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* print_message_function( void* ptr );

main()
{
	pthread_t thread1, thread2;
	const char* message1 = "Thread 1";
	const char* message2 = "Thread 2";
	int iret1, iret2;

	iret1 = pthread_create( &thread1, NULL, print_message_function, (void*)message1 );

	if ( iret1 )
	{
		fprintf( stderr, "Error - pthread_create() return code: %d\n", iret1 );
		exit(EXIT_FAILURE);
	}

	iret2 = pthread_create( &thread1, NULL, print_message_function, (void*)message2 );

	if ( iret1 )
	{
		fprintf( stderr, "Error - pthread_create() return code: %d\n", iret2 );
		exit(EXIT_FAILURE);
	}

	printf( "pthread_create() for thread1 returns: %d\n", iret1 );
	printf( "pthread_create() for thread2 returns: %d\n", iret2 );

	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );

	exit(EXIT_SUCCESS);
}

void* print_message_function( void* ptr )
{
	char* message;
	message = (char*)ptr;
	printf( "%s \n", message);
}
