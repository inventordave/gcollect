// gc_testfncs.c

#include "./../etc.h"
#include "gc.h"


signed INC_ptr_Test( void* ptr ) 	{
	
	if( ptr==NULL )
		return 0;
	
	void* _ = ptr;
	
	_+=1;
	
	char* ds;
	if( systemEndian()==1 /** little-endian */ )
		;

	printf( "ptr++:\tptr=%s.\t(++ptr)=%s.\tsuccess=%d.\n", ptrStr, ptrStr2, result );
	
	return result;
}

