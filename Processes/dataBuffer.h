typedef struct
{
	int* array;
	int* tracker;
	int length;
	int index;
	int readers;
	int buffSM;
	int arraySM;
	int trackSM;
} DataBuffer;

DataBuffer* createBuffer( int, int );

void writeTo( DataBuffer*, int );

int isFull( DataBuffer* );

void freeBuffer( DataBuffer* );
