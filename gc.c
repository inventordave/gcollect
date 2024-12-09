// GC_C

#include <stdlib.h>
#include <stdio.h>
#include "gc.h"

static volatile struct GC* gc;
static volatile struct GC* gc_bkp;
static int z = 0;
static int _entry = 1;

volatile struct GC* initGC( int c )	{

	if( !_entry )	{

		if( gc->_!=NULL )	{

			free( gc->_ );
		}
		free( (struct GC*) gc );
	}

	gc = build_gc_struct( c );

	if( _entry==1 )	{

		gc_bkp = (volatile struct GC*)malloc( sizeof(struct GC) );
		*gc_bkp = *gc;
		_entry=0;
	}

	return (volatile struct GC*) gc;
}
volatile struct GC* build_gc_struct( int c )	{

	volatile struct GC* gc = (volatile struct GC*)malloc( sizeof(volatile struct GC) );

	gc->_ = (void**)calloc( c,sizeof(void*) );

	if( gc->_ == NULL )
		c = 0;

	gc->c = c;
	gc->v = 0;
	gc->size = (int*) calloc( c, sizeof(int) );
	gc->types = (TYPE*) calloc( c, sizeof(TYPE) );
	gc->_v_ = 0;
	gc->_delquote_ = 0;

	return gc;
}

volatile struct GC* getGC()	{

	return gc;
}
int setGC( volatile struct GC* _ )	{

	if( _->_ != NULL )	{

		gc = _;
		return 1;
	}

	return 0;
}

signed getRef( void* ref )	{

	int k;
	for( k=0; k<gc->v; k++ )	{

		if( gc->_[k] == ref )	{
			return k;
		}
	}

	return -1;
}
void* getRef2( int k )	{

	if( gc->v > k )
		if( gc->_ != NULL )
			return gc->_[k];

	return NULL;
}

int freeRef( void* ref )	{

	int k;
	for( k=0; k<gc->v; k++ )	{

		if( gc->_[k] == ref )	{

			free( gc->_[k] );
			gc->_[k] = NULL;
			gc->size[k] = 0;
			gc->types[k] = DELETED;
			gc->_v_--;
			gc->_delquote_++;
			return k;
		}
	}

	return -1;
}
int freeGC( volatile struct GC* gc )	{

	if( gc == NULL )
		return 0;

	if( gc->_ != NULL )
		for( int x=0; x<gc->v; x++ )
			freeRef( gc->_[x] );

	if( gc->size != NULL )
		free( gc->size );

	if( gc->types != NULL )
		free( gc->types );

	int t = gc->_v_;
	free( (struct GC*) gc );

	return t;
}


void* g( void* ref )	{
	
	loop:
	
	if( gc->c > gc->v )	{
		
		(gc->size[gc->v]) = 1;
		gc->_[gc->v++] = ref;
		gc->_v_++;
		return ref;
	}

	report( "The GC object (ref '%p') has been exhausted. Increasing to x2 size (current size = %d, new size = %d).\n", gc, gc->c, gc->c*2 );

	int i;
	if( (i = realloc_gc( gc, gc->c*2 ))<+1 )	{

		report( "Reallocation failed (status code '%d'). Exiting.\n", i );
		fflush( stdout );
		exit( 1 );
	}

	goto loop;

	report( "If you can see this message, the GC has not caught a reference allocation. For assistance "
					"the reference address (&ref) is '%p'.\n", ref );
	
	return ref;
}
void* galloc( int size )	{

	void* _ = g( malloc(size) );

	gc->size[gc->v-1] = size;
	gc->types[gc->v-1] = NOT_SPECIFIED;

	return _;
}
void* galloc2( int size, TYPE type )	{

	void* _ = g( malloc(size) );

	gc->size[gc->v-1] = size;
	gc->types[gc->v-1] = type;

	return _;
}
void* galloc3( TYPE type )	{

	int s = sizeof_type(type);
	void* _ = g( malloc( s ) );

	gc->size[gc->v-1] = 1;
	gc->types[gc->v-1] = type;

	return _;
}

char* gcchar( void* ref )	{

	return (char*) galloc3( CHAR_PTR );
}
char* gcchar2( int length )	{

	return (char*) galloc2( length, CHAR_PTR );
}

int gc_status()	{

	return (gc->v < gc->c);
}
int reset_gc_ptr( )	{

	if( !_entry )	{
		*gc = *gc_bkp;
		return 1;
	}

	return 0;
}
int realloc_gc( volatile struct GC* gc_, int c2 )	{

	if( gc_ == NULL )
		gc_ = gc;

	int c = gc_->c;
	int _v_ = gc->_v_;

	if( c2 == c )
		return 0;

	int t;
	if ( c2 < c )
		t = c2;
	else
		t = c;

	void** _ = (void**)malloc( sizeof(void*) * c );

	int i;

	for( i=0; i < t; i++ )
		_[i] = gc_->_[i];

	free( gc_->_ );

	gc_->_ = (void**)malloc( sizeof(void*) * c2 );

	for( i=0; i < t; i++ )
		gc_->_[i] = _[i];
	
	gc_->v = i;
	gc_->_v_ = _v_;
	
	if( i < c )	{

		free( _ );
		return -1; 
	}

	for( ; i < c2; i++ )
		gc_->_[i] = (void*)calloc( 1, sizeof(void*) );

	gc_->c = i;

	free( _ );

	return +1;
}

struct gc_report cleanUp( )	{

	int total = gc->_delquote_;
	int remaining = freeGC( gc );

	struct gc_report _;
	_.totalDeleted = total+remaining;
	_.v = remaining;
	return _;
}

// For user-defined struct's, the user must use the canonical sizeof(), as so: sizeof( struct StructTag ), where
// StructTag represents the struct TypeName.
signed sizeof_type( TYPE type )	{

	switch( type )	{

		// NOT_SPECIFIED OR DELETED.
		break;
		case NOT_SPECIFIED:
			return 0;
			break;

		break;
		case DELETED:
			return -1;


		// VOID_PTR CLASSES.
		break;
		case VOID_PTR:
			return sizeof( void* );
			break;

		break;
		case VOID_PTR_PTR:
			return sizeof( void** );
			return 0;
			break;


		// INT.
		break;
		case INT:
			return sizeof( int );
			break;

		break;
		case UNSIGNED_INT:
			return sizeof( unsigned );
			break;

		break;
		case SIGNED_INT:
			return sizeof( signed );
			break;

		break;
		case INT_PTR:
			return sizeof( int* );
			break;

		break;
		case UNSIGNED_INT_PTR:
			return sizeof( unsigned* );
			break;

		break;
		case SIGNED_INT_PTR:
			return sizeof( signed* );
			break;

		break;
		case INT_PTR_PTR:
			return sizeof( int** );
			break;
		break;
		case UNSIGNED_INT_PTR_PTR:
			return sizeof( unsigned** );
			break;
		break;
		case SIGNED_INT_PTR_PTR:
			return sizeof( signed** );
			break;
		break;

		break;
		case LONG_INT_PTR:
			return sizeof( long* );
			break;

		break;
		case UNSIGNED_LONG_INT_PTR:
			return sizeof( unsigned long* );
			break;

		break;
		case SIGNED_LONG_INT_PTR:
			return sizeof( signed long* );
			break;

		break;
		case LONG_INT_PTR_PTR:
			return sizeof( int** );
			break;
		break;
		case UNSIGNED_LONG_INT_PTR_PTR:
			return sizeof( unsigned long** );
			break;
		break;
		case SIGNED_LONG_INT_PTR_PTR:
			return sizeof( signed long** );
			break;
		break;

		break;
		case LONG_LONG_INT:
			return sizeof( long long int );
			break;

		break;
		case UNSIGNED_LONG_LONG_INT:
			return sizeof( unsigned long long );
			break;

		break;
		case SIGNED_LONG_LONG_INT:
			return sizeof( signed long long );
			break;

		break;
		case LONG_LONG_INT_PTR:
			return sizeof( long long int * );
			break;

		break;
		case UNSIGNED_LONG_LONG_INT_PTR:
			return sizeof( unsigned long long * );
			break;

		break;
		case SIGNED_LONG_LONG_INT_PTR:
			return sizeof( signed long long * );
			break;

		break;
		case LONG_LONG_INT_PTR_PTR:
			return sizeof( long long int ** );
			break;

		break;
		case UNSIGNED_LONG_LONG_INT_PTR_PTR:
			return sizeof( unsigned long long ** );
			break;

		break;
		case SIGNED_LONG_LONG_INT_PTR_PTR:
			return sizeof( signed long long ** );
			break;


		// CHAR.
		break;
		case CHAR:
			return sizeof( char ); // STD C guarantees this to be size(1)
			break;

		break;
		case UNSIGNED_CHAR:
			return sizeof( unsigned char ); // STD C guarantees this to be size(1)
			break;

		break;
		case SIGNED_CHAR:
			return sizeof( signed char );
			break;

		break;
		case CHAR_PTR:
			return sizeof( char* );
			break;

		break;
		case UNSIGNED_CHAR_PTR:
			return sizeof( unsigned char * );
			break;

		break;
		case SIGNED_CHAR_PTR:
			return sizeof( signed char * );
			break;

		break;
		case CHAR_PTR_PTR:
			return sizeof( char** );
			break;

		break;
		case UNSIGNED_CHAR_PTR_PTR:
			return sizeof( unsigned char ** );
			break;

		break;
		case SIGNED_CHAR_PTR_PTR:
			return sizeof( signed char ** );
			break;


		// FLOAT.
		break;
		case FLOAT:
			return sizeof( float );
			break;

		break;
		case UNSIGNED_FLOAT:
			return sizeof( float );
			break;

		break;
		case SIGNED_FLOAT:
			return sizeof( float );
			break;

		break;
		case FLOAT_PTR:
			return sizeof( float* );
			break;

		break;
		case UNSIGNED_FLOAT_PTR:
			return sizeof( float * );
			break;

		break;
		case SIGNED_FLOAT_PTR:
			return sizeof( float * );
			break;

		break;
		case FLOAT_PTR_PTR:
			return sizeof( float** );
			break;

		break;
		case UNSIGNED_FLOAT_PTR_PTR:
			return sizeof( float ** );
			break;

		break;
		case SIGNED_FLOAT_PTR_PTR:
			return sizeof( float ** );
			break;


		// DOUBLE.
		break;
		case DOUBLE:
			return sizeof( double );
			break;

		break;
		case UNSIGNED_DOUBLE:
			return sizeof( double );
			break;

		break;
		case SIGNED_DOUBLE:
			return sizeof( double );
			break;

		break;
		case DOUBLE_PTR:
			return sizeof( double* );
			break;

		break;
		case UNSIGNED_DOUBLE_PTR:
			return sizeof( double * );
			break;

		break;
		case SIGNED_DOUBLE_PTR:
			return sizeof( double * );
			break;

		break;
		case DOUBLE_PTR_PTR:
			return sizeof( double** );
			break;

		break;
		case UNSIGNED_DOUBLE_PTR_PTR:
			return sizeof( double ** );
			break;

		break;
		case SIGNED_DOUBLE_PTR_PTR:
			return sizeof( double ** );
			break;


		// STRUCT_PTR CLASSES.
		break;
		case STRUCT_PTR:
			return sizeof( void* );
			break;

		break;
		case STRUCT_PTR_PTR:
			return sizeof( void** );
			break;

		break;
		default:
			return -2;
			break;

		break;
	}

	return -3;
}

// EOF
