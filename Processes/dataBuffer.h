typedef struct
{
	int* array;
	int* tracker;
	int length;
	int index;
	int readers;
} DataBuffer;

DataBuffer* createBuffer( int, int );

void writeTo( DataBuffer*, int );

int isFull( DataBuffer* );

void freeBuffer( DataBuffer* );
