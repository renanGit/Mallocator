#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
// For open
#include <fcntl.h>

// For mmap
#include <sys/mman.h>

// Caches are:
// 16, 32, 64, 128, 256, 512, 1024, 2048

#include "memory.h"
#include "bits.h"

//TO BE MOVED TO TEST 5
#define NUM_THREADS 10
#define MAX 100000

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int fine = 1;
unsigned int counter[NUM_THREADS];
void* addr[NUM_THREADS];
pthread_t thread[NUM_THREADS];

// File descriptor for zeroes file
int fd;

Memory M;

void allocator_init() {

	fd = open("/dev/zero", O_RDWR);

	// fd = open("../ZEROES", O_RDWR);

	if(fd == -1) {
		perror("File open failed");
		exit(0);
	}

	// Initialize the region list
	M.R = NULL;

	int i;

	// Initialize the caches
	for (i = 0; i < 8; i++) {
		M.C[i].size = 16 << i;
		M.C[i].S = NULL;
	}

	return;
}


void allocator_finalize() {

	close(fd);

	// maybe do a few other things
	// homework: lookup how init and
	// finalize work for dynamically
	// linked libraries 
	// Use that correctly! 

}

void *allocate_region (unsigned int size) {
	
	Region *region, *temp;

	temp = M.R;

	void *mapped_addr = mmap(NULL, size + REGION_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

	if(mapped_addr == MAP_FAILED) {
		perror("Mapping failed");
		exit(0);
	}
	// mmap return addr
	region = mapped_addr;
	// the size of the alloc
	region->size = size;
	// the mapped addr + the region_size (Struct)
	region->addr = mapped_addr + REGION_SIZE;

	// the initialize
	M.R = region;
	
	region->next = temp;

	return region->addr;
}

int getset_free_slot(Slab* S) {

	if(S == NULL){ return -1; }
	int free_slot = -1;

	int chars = S->bitmap_size;
	int i;	// go through the amount of bitmapps

	// iterate through every bitmap to find a free slot
	for(i = 0; i < chars; i++) {
		// returns a -1 on fail; 0 on success
		free_slot = getset_free_bit(S->bitmap + i, S->bitmap_size, S->slots); 
		
		if(free_slot != -1) {
			// the offset
			free_slot = i * 8 + free_slot; 
			break;
		}
	}
	
	// since some of the bits in the 
	// last char may not be usable
	// owing to insufficient slots
	if(free_slot >= S->slots){ return -1; }

	return free_slot;
}

void* allocate_cache(unsigned int size) {

	int ci = 0;
	Slab* iterator = M.C[ci].S;
	Slab* S;
	int slot;
	
	// 1. Identify which cache to use
	while ((size-1) >> (ci+4)){ ci++; }

	// 2. Identify which slab to use
	// Find a slab in the slab list 
	// that has an empty slot
	// 3. Identify which slot to use
	// 4. Mark slot as used

	// search for a free slab
	while((slot = getset_free_slot(iterator)) == -1) {
		if(iterator == NULL) break;
		iterator = iterator->next;
	}
	// no empty slab 
	if(iterator == NULL) {
		
		// Allocate a slab
		void *mapped_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		if(mapped_addr == MAP_FAILED) {
			perror("Mapping failed");
			return NULL;
		}

		S = mapped_addr;
		// M.C[ci].size has the Cache size (16 32 64 128...)
		S->slot_size = M.C[ci].size;
		
		// offset to get the bitmap addr
		S->bitmap = mapped_addr + SLAB_SIZE;
		
		// (4096 - Slab struct (28bytes))*(8bits * Cache size) / (8bits * Cache size + 1byte)
		S->size = (PAGE_SIZE-SLAB_SIZE)*(8*S->slot_size)/(8*S->slot_size + 1);
		
		// the actual size (Page size - Slab struct) / Cache size
		S->slots = S->size / S->slot_size;
	
		// if not a multiple of 8 then add another byte
		// else the size the bitmap will be divided by 8
		if(S->slots % 8 != 0){ S->bitmap_size = S->slots / 8 + 1; }
		else{ S->bitmap_size = S->slots / 8; }
		
		// offset the bitmap and bitmap size to get the addr of the first slot
		S->addr = S->bitmap + S->bitmap_size;

		// Enqueue a new slab
		if(M.C[ci].S != NULL){
			// have S (S is the simi head) point to head
			S->next = M.C[ci].S;
			// change the head to have the simi head value
			M.C[ci].S = S;
		}
		else{ 
			S->next = NULL; 
			// intialize head
			M.C[ci].S = S;
		}
		// 
		slot = getset_free_slot(S);
	} // The address to the slab is S
	else{ S = iterator; }

	// 4. Compute and return address of the slot
	// offset( first slot address + free slot position from the bitmap * the Cache size)
	return (S->addr + slot*S->slot_size);
}

void *allocate(unsigned int size) {

	if(size == 0) {
		printf("Requested allocation of zero size!\n");
		return NULL;
	}

	if(size > MAX_SIZE) {
		printf("Allocation request too big!\n");
		return NULL;
	}
	// slab allocation; region allocation
	if(size <= 2048){ return allocate_cache(size); }
	else { return allocate_region(size); }
}

int deallocate_region(void* ptr) {

	int found = 0;
	// get the mapped addr by subtracting the region struct
	void *mapped_addr = ptr - REGION_SIZE;

	// the head of the region
	Region *iterator = M.R;
	Region *prev = M.R;

	// head not null
	while(iterator != NULL) {
		// if addr is equal to the addr, then found is set
		// there are not slots just one addr
		if (iterator->addr == ptr) {
			found = 1;
			break;
		}
		prev = iterator;
		iterator = iterator->next;
	} 
	
	// not found ret fail -1
	if(!found){ return -1; }

	// success then remove the region from the list
	if(iterator == M.R) { M.R = iterator->next; }
	else{ prev->next = iterator->next; }

	// get the mapped addr
	int mapped_size = iterator->size + REGION_SIZE;

	// Use munmap to return memory to OS
	if(munmap(mapped_addr, mapped_size) == -1){ perror("MUNMAP"); }

	return 0;
}


int deallocate_cache(void* ptr) {
	
	// 1. Find the cache
	// 2. Find the slot address should match with the
	// starting address of slot 
	// if not fail the deallocation request and return 
	
	int found = 0;	// found the addr that needs to be dealloc
	int ci = -1;	// cache index
	// offset used to get which index in a byte
	// offset1 used to get which table index from the bitmap
	// reszero tests if bitmap at 'i' is all zero
	int offset, offset1, i, reszero = 0;
	// offset in bits? to addr (first addr)
	unsigned long offsetB_toaddr;
	// temp holds the found addr; previous of the previous is to see if we only have one node
	// to set the head to null indicating that we did a unmap
	// prev is the previous element; iterator leads
	Slab *temp, *p_prev = NULL, *prev, *iterator;

	// loop through the caches
	while(!found && ci < 8){
		++ci;
		// set the iterator and the prev to head 
		prev = iterator = M.C[ci].S;
		
		// loop through the slabs
		while(iterator != NULL && !found){
			
			// when prev is not equal to iterator then move up p_prev
			if(prev != iterator){ p_prev = prev; }
			
			// addr of the first slot 
			if (iterator->addr <= ptr) {
				// get the offset to the first addr
				offsetB_toaddr = (ptr - iterator->addr);
				// check if the ptr belongs to the addr slab if not then 
				// it ignores the ptr 
				if((ptr - offsetB_toaddr) == iterator->addr && offsetB_toaddr < 4096){
					found = 1;
					temp = iterator;
					break;
				}
			}
			// set the prev; increment to next slab
			prev = iterator;
			iterator = iterator->next;
		}
	}

	if (!found){ printf("Double Free Error!\n"); return -1; }
	
	// 3. Check if bit was set in bitmap
	// if yes, deallocation is possible
	// if not, fail deallocation request and return
	// 4. reset bit (corresponding to slot) in bitmap
	// 5. release memory if necessary
	
	// offset to the first addr
	offset = (ptr - temp->addr) / temp->slot_size;
	// get the offset in bytes
	offset1 = (offset) / 8;
	
	// check if bit is 0 ret -1 fail; reset the bit on success
	if(getbit(*(temp->bitmap + (offset1 % temp->bitmap_size)), (offset % 8)) == 0){ printf("Double Free Error!\n"); return -1; }
	else{ resetbit((temp->bitmap + (offset1 % temp->bitmap_size)), (offset % 8)); }
	
	
	// for loop the bitmap check if all zeroes
	for(i = 0; i < temp->bitmap_size; i++){		
		reszero = zero_bitmap(*(temp->bitmap + i));
		if(!reszero){ break; }	
	}
	
	if(reszero){ 
		// the size is the cache size (16,32,..) + the slab struct
		int mapped_size = temp->size + SLAB_SIZE;
		
		// if p_prev is null then set head to null
		if (p_prev == NULL){ iterator = M.C[ci].S = iterator->next;	}
		else if(iterator->next != NULL){
			// cut the middle slab out
			prev->next = iterator->next;
			// move prev up
			iterator = prev;			
		}
		else { prev->next = NULL; }
		// Use munmap to return memory to OS
		if(munmap((temp->bitmap - SLAB_SIZE), mapped_size) == -1){ perror("MUNMAP"); }
	}
	return 0;
}

void deallocate(void* ptr) {

	// deallocate from region
	if(deallocate_region(ptr) == -1) {
		// deallocate from cache
		if(deallocate_cache(ptr) == -1) {
			printf("DEALLOCATE: Allocation not found @ %p!\n", ptr);
			return;
		}
	}

	//printf("Deallocated %p\n", ptr);

	return;
}

