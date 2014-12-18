#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "memory.h"
#define NUM_THREADS 10
#define LOOP_MAX 100000
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int fine = 1;
unsigned int counter[NUM_THREADS];
void* addr[ NUM_THREADS ];
void *Bytes256[ LOOP_MAX ]; // will allocate the 256 Bytes --> 100,000 times  
pthread_t thread[ NUM_THREADS ];

void test1( ){

    // STEP 1: Make an allocation with a size of 32 bytes for a pointer of char* type.
    // STEP 2: then assign some value through the pointer and output them.
    // STEP 3: deallocate it once.
    
    printf( "****** Test 1 ******\n\n" );
    
    char *Bytes32 = ( char * ) allocate( 32 );
    char *str = "helloWorld helloWorld helloWorl"; //has 31 character inside
    int i = 0;
    for ( i = 0; i < 32; i++) {
        Bytes32[i] = str[i];
    }

    printf("Characters: %s\n\n", Bytes32 );
    
    deallocate( Bytes32 );    
        
    printf( "******* End of test 1 *******\n");
}

void test2( ){

	// STEP 1: Make an allocation with a size of 32 bytes 
	// STEP 2: deallocated twice.( double Free ).

	printf( "****** Test 2 ******\n\n" );

	void *Bytes32 = ( char * ) allocate( 32 );
	
	deallocate( Bytes32 );
	deallocate( Bytes32 );// can not deallocate twice
	   
	printf( "****** End of test 2 *******\n");
}


void test3( ){

    // STEP 1: Make an allocation with a size of 32 bytes, 64 Bytes, 128 Bytes
    //         192 Bytes, 256 Bytes
    // STEP 2: deallocated them.
    
    printf( "****** Test 3 ******\n\n" );
    
    void *Bytes32  = (void *)allocate( 32 );
    void *Bytes64  = (void *)allocate( 64 );
    void *Bytes128 = (void *)allocate( 128 );
    void *Bytes192 = (void *)allocate( 192 );
    void *Bytes256 = (void *)allocate( 256 );
    
    deallocate( Bytes32 );
    deallocate( Bytes64 );
    deallocate( Bytes128 );
    deallocate( Bytes192 );
    deallocate( Bytes256 );    
    
    printf( "****** End of test 3 ******\n");
}

void test4( ){

    // STEP 1: Loop 20 times and in each loop make an allocation with a size of
    //         32 Bytes. 
    // STEP 2: When the loop is over, deallocate them.
    
    printf( "****** Test 4 ******\n\n" );
    void *Bytes32[ 20 ];
    
    int i = 0;    
    
    // Allocation
    for( i = 0; i < 20; i++ ){
         Bytes32[ i ] = (void *)allocate( 32 );
    }
    
    // Dealocation
    for( i = 0; i < 20; i++ ){
        deallocate( Bytes32[ i ] );
    }
    printf( "****** End of test 4 ******\n");
}

// executes the allocation loop performed by each thread
void *test5Helper(void * arg ){
   
	if(!fine){ pthread_mutex_lock(&mutex1); }

	int i, j, k; 
     void * ptr;	
	
	long int id = (long int)arg;
	
	for(i = 0; i < LOOP_MAX; i++){
		
		if(fine){ pthread_mutex_lock(&mutex2); }	
		
		ptr = (void *)allocate(256);
		//addr[ id ] = ptr;
		Bytes256[ i ] = ptr;
		if(fine){ pthread_mutex_unlock(&mutex2); }
		
		//compare the address to other addresses
		for (j = 0; j < NUM_THREADS; j++){
			for(k = j+1; k <= NUM_THREADS; k++){
				if(addr[j] == addr[k] && addr[k] != 0 && addr[j] != 0 && j != k){
					puts("Two threads used the same address!");
				}
			}
		}	
	}
	if(!fine){ pthread_mutex_unlock(&mutex1); }
	
	printf( "Done threading %d\n\n", i+1 ); 

	return NULL;
}

void test5( ){

    // STEP 1: Create 10 threads and in each thread loop 100,000 times and 
    //         in each loop make an allocation with size of 256 Bytes.
    // STEP 2: Deallocate them. ( Make sure that no two threads get the same
    //         address returned
    
	printf( "****** Test 5 ******\n\n" );

	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	
	pthread_t threads[ NUM_THREADS ];
	int i = 0;
	int err = 0;

	for(i = 0; i < NUM_THREADS; i++){ addr[i] = 0; }

	for( i = 0; i < NUM_THREADS; i++ ){
	
	   int *ti = ( int * ) allocate( sizeof( int ) );
	   *ti = i;
	   
	   printf( "Theads[ %d ]\n", i );
	   err = pthread_create( &threads[ i ], NULL, test5Helper, ( void * )(i ) );
	   
	   if( err != 0 ){
		  perror( "Thread creation failed!" );
		  exit( 1 );            
	   }
	}

	for (i = 0; i < LOOP_MAX; i++) {
		pthread_join(threads[i], NULL );
	}
	
	printf( " \n    ALLOCATE IS DONE.\n**DEALOCATE WILL BEGIN**\n\n");

	for( i = 0; i < LOOP_MAX; i++ ){
		deallocate( Bytes256[ i ] );    
	} 
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	printf( "****** End of test 5 ******\n");
}

void test6( ){

    // STEP 1: Continously make allocations of a random size from 1 Byte to 
    //         256 Bytes.
    // STEP 2: Count the total number of bytes allocated until an allocation
    //         fails. 
    // STEP 3: then output  the result.
    
    
    printf( "****** Test 6 ******\n\n" );
    
     // 190811109 bytes = 190.8 MB = 0.1908 GB
	// time 5m27.455s
	unsigned int sum = 0;
	int random_number;
	srand ( time(NULL) );
	while(1){
		
		random_number = (rand() % 256) + 1; 
		
		if(allocate(random_number) != NULL){ sum += random_number; }
		else{ break; }
	}
    printf( "The allocation count is: %d Bytes\n", sum );
    int mb = sum / 1048576;
    printf( "Allocation in MB: %d\n\n", mb );
    printf( "****** End of test 6 ******\n");
}


void runTime( void test( ) ){

    clock_t start, stop;
    double t = 0.0;
    
    // Start timer
    assert( ( start = clock( ) ) != -1 );

    test( );

    // Stop timer
    stop = clock( );
    
    t = ( double )( stop - start ) / CLOCKS_PER_SEC;
    printf( "Run time: %4.3f\n\n", t );
}


int main( int argc, char *argv[ ] ){

	allocator_init();
	int testNumber = 0;

	if( argc != 2 ){
		printf("Usage: %s | 1 | 2 | 3 | 4 | 5 | 6 |\n", argv[ 0 ] );
		return 0;
	}

	testNumber = atoi(argv[1]);

	if ( ( testNumber < 1 ) || ( testNumber > 6 )  ) {
		printf("\nError.\nOption %d is an invalid token!.\n", testNumber );
		printf( "Please choose a test number between 1 and 6 \n\n" );
		return 0; 
	}

	if ( testNumber == 1 ) { runTime( test1 ); } 
	else if ( testNumber == 2 ){ runTime( test2 ); } 
	else if ( testNumber == 3 ){ runTime( test3 ); }
	else if ( testNumber == 4 ){ runTime( test4 ); }         
	else if ( testNumber == 5 ){ runTime( test5 ); } 
	else if ( testNumber == 6 ){ runTime( test6 ); }
	
	allocator_finalize();
	
	return 0;
}
