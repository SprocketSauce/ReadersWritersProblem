typedef struct
{
	FILE* inFile;
	FILE* outFile;
	int waitTime;
	DataBuffer* buffer;
} WriterInput;

typedef struct
{
	FILE* outFile;
	int waitTime;
	DataBuffer* buffer;
} ReaderInput;

void* reader( void* );

void* writer( void* );

void printToSimOut( FILE*, char[100] );
