// C GC utility functions & struct.

#ifndef _DAVELIB_C_GC_
#define _DAVELIB_C_GC_

// LEXICAL SUGAR
#ifndef report
#define report printf
#endif


// ** GC CONTEXT STRUCTURES ** //

typedef enum {

	NOT_SPECIFIED,
	DELETED,


	VOID_PTR,
	VOID_PTR_PTR,


	INT,
	UNSIGNED_INT,
	SIGNED_INT,

	INT_PTR,
	UNSIGNED_INT_PTR,
	SIGNED_INT_PTR,
	
	INT_PTR_PTR,
	UNSIGNED_INT_PTR_PTR,
	SIGNED_INT_PTR_PTR,


	LONG_INT,
	UNSIGNED_LONG_INT,
	SIGNED_LONG_INT,

	LONG_INT_PTR,
	UNSIGNED_LONG_INT_PTR,
	SIGNED_LONG_INT_PTR,

	LONG_INT_PTR_PTR,
	UNSIGNED_LONG_INT_PTR_PTR,
	SIGNED_LONG_INT_PTR_PTR,


	LONG_LONG_INT,
	UNSIGNED_LONG_LONG_INT,
	SIGNED_LONG_LONG_INT,
	
	LONG_LONG_INT_PTR,
	UNSIGNED_LONG_LONG_INT_PTR,
	SIGNED_LONG_LONG_INT_PTR,

	LONG_LONG_INT_PTR_PTR,
	UNSIGNED_LONG_LONG_INT_PTR_PTR,
	SIGNED_LONG_LONG_INT_PTR_PTR,


	CHAR,
	UNSIGNED_CHAR,
	SIGNED_CHAR,

	CHAR_PTR,
	UNSIGNED_CHAR_PTR,
	SIGNED_CHAR_PTR,

	CHAR_PTR_PTR,
	UNSIGNED_CHAR_PTR_PTR,
	SIGNED_CHAR_PTR_PTR,


	FLOAT,
	UNSIGNED_FLOAT,
	SIGNED_FLOAT,

	FLOAT_PTR,
	UNSIGNED_FLOAT_PTR,
	SIGNED_FLOAT_PTR,

	FLOAT_PTR_PTR,
	UNSIGNED_FLOAT_PTR_PTR,
	SIGNED_FLOAT_PTR_PTR,


	DOUBLE,
	UNSIGNED_DOUBLE,
	SIGNED_DOUBLE,

	DOUBLE_PTR,
	UNSIGNED_DOUBLE_PTR,
	SIGNED_DOUBLE_PTR,

	DOUBLE_PTR_PTR,
	UNSIGNED_DOUBLE_PTR_PTR,
	SIGNED_DOUBLE_PTR_PTR,


	STRUCT,
	STRUCT_PTR,
	STRUCT_PTR_PTR

} TYPE;

typedef struct GC	{

	void** _;
	int c;
	int v;
	signed* size;
	TYPE* types;

	int _v_;
	int _delquote_;

} GC;

typedef struct gc_report	{

	int totalDeleted;
	int v;

} gc_report;


// ** GC FUNCTIONS (OPERATORS). ** //

// The int parameter is a MAX value for the number of ptrs to store. Set a reasonable 
extern volatile struct GC* initGC( int );

// As with initGC( int ), the int parameter is the MAX num of ptrs. Use this function for generating
// more GC data structures.
extern volatile struct GC* build_gc_struct( int );

// Returns a reference to the Active GC data structure.
extern volatile struct GC* getGC();

// Sets the Active GC context. This is also done implicitly on a load-time call to fnc: initGC(int).
// Returns 1 on SUCCESS, 0 on FAILURE.
// It will fail if the GC* structure is not initialised properly. Use fnc: build_gc_struct( int ) to initialize a GC structure.
extern int setGC( volatile struct GC* );


// For user-defined struct's, the user must use the canonical sizeof(), as so: sizeof( struct StructTag ),
// where 'StructTag' represents the struct Type name. (e.g. "MyCustomStructType" )
signed sizeof_type( TYPE );


// Saves a copy of heap-allocated reference, and returns a copy of the reference.
// Default Usage: g( ref )
extern void* g( void* );

// Custom malloc()
#define malloc gc_malloc
extern void* gc_malloc( int v );

#define calloc gc_calloc
extern void* gc_calloc( int width, int v );

#define alloc gc_alloc
extern void* gc_alloc( int width, int v, char c );


// Helper. returns a new reference pre-staticcasted to (char*).
extern char* gcchar( void* );

// These 4 store more information, and do the malloc() allocation internally.
extern void* galloc( int buffer_size );

// The return allocated ptr needs to be static-casted by the User to the intrinsic Type represented by 'storage_class'.
// See 'enum {...} TYPE' further up this .header file.
extern void* galloc2( int buffer_size, TYPE storage_class );

// Allocates a buffer to store 1 instance of Type represented by Arg 'TYPE type' ).
// User still needs to static-cast the returned pointer, for now.
extern void* galloc3( TYPE type );


// Will return the index of the reference in the GC Active Context if is stored therewithin.
// On being unable to find the reference, will return -1.
signed getRef2( void* );

// returns the reference stored at indice [int]. If no reference is found at such an index, returns NULL.
void* getRef( int );

// Returns the new ptr value, or NULL if ptr cannot be found in the gc ActiveContext.
void* UpdateGC( void* ptr, void* newptr );
#define INC_PTR(ptr) UpDateGC(ptr,(++ptr));

// self-explanatory.
// Searches the Active GC Context.
int freeRef( void* );

// Helper to free any GC contexts you have initialised with fnc:
// GC_* build_gc_struct(int);
extern int freeGC( volatile struct GC* gc );


// Returns 1 if there is space left in the Active GC to add a reference. 0 if there is no more space.
extern int gc_status();

// resizes either the GC reference passed as Arg 1, or the default GC object if NULL is passed.
// Arg 2 [int] is the new size.
extern int realloc_gc( volatile struct GC* _, int c );

// Will reset the Active GC structure reference to the default object created upon initializing DAVELIB GC with initGC(...).
// Returns 1 on success, or 0 on failure. It will fail if initGC(...) has not been called at least once during the process lifetime.
extern int reset_gc_ptr();


// This only cleans up the Active GC context. If you have made multiple GC data structures, they will need to be
// freed seperately, using fnc: freeGC(...) 
extern gc_report cleanUp(void);

#endif

// EOF
