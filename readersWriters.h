typedef struct
{
	FILE* inFile;
	FILE* outFile;
	int waitTime;
	int readers;
	int* reading;
	int* writing;
	DataBuffer* buffer;
} writeIn;

typedef struct
{
	FILE* outFile;
	int waitTime;
	int* reading;
	int* writing;
	DataBuffer* buffer;
} readIn;a
