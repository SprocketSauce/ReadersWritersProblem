typedef struct
{
	sem_t mutex;
	sem_t outMutex;
	sem_t cond;
} RWSemaphores;

typedef struct
{
	int sdLength;
	int eof;
	int reading;
} Flags;

void* reader( void* );

void* writer( void* );

void printToSimOut( FILE*, char[100] );
