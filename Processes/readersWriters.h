typedef struct
{
	sem_t mutex;
	sem_t wrt;
	sem_t outMutex;
	sem_t cond;
	sem_t done;
} RWSemaphores;

typedef struct
{
	int wIndex;
	int reading;
	int writing;
	int finished;
} Flags;

void reader( RWSemaphores*, DataBuffer*, Flags*, int, int, int );

void writer( RWSemaphores*, DataBuffer*, Flags*, int, int, int* );

void printToSimOut( RWSemaphores*, char[100] );

void markFinished( RWSemaphores*, Flags*, int );
