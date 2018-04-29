typedef struct
{
	int* array;
	int* tracker;
	int length;
} DataBuffer;

DataBuffer* createBuffer( int );

void writeTo( DataBuffer*, int, int );

int readFrom( DataBuffer*, int );

void freeBuffer( DataBuffer* );
