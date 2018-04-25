typedef struct
{
	int* array;
	int length;
	int start;
	int end;
} CircularBuffer;

CircularBuffer* createBuffer( int );

int writeTo( CircularBuffer*, int );

int readFrom( CircularBuffer* );

int isFull( CircularBuffer* );

int isEmpty( CircularBuffer* );

void freeBuffer( CircularBuffer* );
