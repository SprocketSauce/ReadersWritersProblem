#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "readersWriters.h"
#include "fileIO.h"

int main( int argc, char* argv[] )
{
	ReaderInput readIn;
	WriterInput writeIn;
	int r, w, rcount, wcount;
	FILE* file;

	file = openFile( argv[1] );
	writeIn.file = file;
	r = argv[2];
	w = argv[3];
	readIn.wait = argv[4];
	writeIn.wait = argv[5];
	readIn.readers = r;
	writeIn.readers = r;

	wcount = 0;
	rcount = 0;
}
