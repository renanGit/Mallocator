#include <stdio.h>
#include <assert.h>
#define MSB_MASK (1<<(sizeof(char)*8-1))

void printbits(char c) {

	int i;
	for(i = 0; i < (int)sizeof(char)*8; i++) {

		// one way to do this
		if(c & MSB_MASK) {
			printf("1");
		} 
		else {
			printf("0");
		}

		// another way to do this
		// printf("%d", (int)((c & MSB_MASK)>>(sizeof(char)*8-1));

		//c = c << 1;
		c <<= 1;

	}  

	printf("\n");

}


// Set a bit 
void setbit(char *c, int pos) {
	char mask = 1 << pos;
	*c = (*c) | mask;
}



// Reset a bit
void resetbit(char *c, int pos) {
	char mask = ~(1 << pos);
	*c = (*c) & mask;
}

// Get a bit

int getbit(char c, int pos) {
	
	char mask = 1 << pos;
	if(c & mask) return 1;
	else return 0;

	// also possible
	// return c&mask?1:0;
}

int zero_bitmap(char c){
	char mask = 0;
	return (c | mask ? 0 : 1);
}


// Toggle a bit 
void togglebit(char *c, int pos) {

	// check if position is valid
	// terminate program if not. 
	assert(pos<8);

	if(getbit(*c,pos)==0) {
		setbit(c,pos);
	} 
	else {
		resetbit(c,pos);
	}
}


// Finds a zero bit within a char
// returns the position of the zero bit if found
// Else returns -1

int getset_free_bit(char* c, int bitmap_size, int slots) {
	int find_size;// = slots / bitmap_size;
	int ret = -1;
	int i;

	if(slots % bitmap_size != 0){ find_size = slots / bitmap_size + 1; }
		else{ find_size = slots / bitmap_size; }

	for(i = 0; i < (int)sizeof(char)*find_size; i++) {
		if(getbit(*c, i) == 0) {
			setbit(c, i);
			ret = i;
			break;
		}
	}

	return ret;
}

// Count the number of 1s

// Swap 2 bits

// Reverse bits




