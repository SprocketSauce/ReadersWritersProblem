typedef struct
{
	sem_t mutex;
	sem_t wrt;
	sem_t outMutex;
	sem_t cond;
} RWSemaphores;

typedef struct
{
	int wIndex;
	int reading;
	int writing;
} Flags;

void* reader( void* );

void* writer( void* );

void printToSimOut( FILE*, char[100] );
