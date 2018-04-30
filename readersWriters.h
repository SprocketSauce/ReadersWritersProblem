typedef struct
{
	FILE* inFile;
	FILE* outFile;
	int waitTime;
	int readers;
	int* reading;
	int* writing;
	DataBuffer* buffer;
} WriterInput;

typedef struct
{
	FILE* outFile;
	int waitTime;
	int* reading;
	int* writing;
	DataBuffer* buffer;
} ReaderInput;

void* reader( void* );

void* writer( void* );
