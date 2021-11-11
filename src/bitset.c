#include "bitset.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct bitset {
	/* the size of the bitset in bits, not the size of the array in bytes */
	size_t size;
	uint8_t array[];
};

/* find the byte position that holds a given bit */
size_t byte_of_bit(size_t);

/* the minimum size in bytes to hold a given number of bits */
size_t min_size(size_t);

/* increase to the next even multiple of 8 */
size_t round_to_8(size_t);

/******************************************************************************/

size_t byte_of_bit(size_t x) {
	return x >> 3;
}

size_t find_clear_bit(BITSET* b, size_t start) {
	assert(start < b->size);
	for (size_t i = start; i < b->size; i++) {
		if (!get_bit(b, i)) return i;
	}
	return -1;
}

uint8_t get_bit(BITSET* b, size_t index) {
	assert(index < b->size);
	return b->array[byte_of_bit(index)] & (1 << (index & 7));
}

BITSET* make_bitset(void* buffer, size_t buffer_size, size_t size) {
	size_t minsize = sizeof (struct bitset) + min_size(size);
	assert(buffer_size >= minsize);
	memset(buffer, 0, minsize);

	BITSET* b = buffer;
	b->size = size;
	return b;
}

size_t min_size(size_t x) {
	return byte_of_bit(round_to_8(x));
}

BITSET* new_bitset(size_t size) {
	size_t bufsize = sizeof (struct bitset) + min_size(size);
	return make_bitset(malloc(bufsize), bufsize, size);
}

void print_bitset(FILE* f, BITSET* b) {
	for (size_t i = 0; i < b->size; i++) {
		fprintf(f, "%c", get_bit(b, i) ? '1' : '0');
	}
}

size_t round_to_8(size_t x) {
	switch (x & 7) {
		case 0: return x;
		case 1: return x + 7;
		case 2: return x + 6;
		case 3: return x + 5;
		case 4: return x + 4;
		case 5: return x + 3;
		case 6: return x + 2;
		case 7: return x + 1;
	}
}

void set_bit(BITSET* b, size_t index) {
	assert(index < b->size);
	b->array[byte_of_bit(index)] |= 1 << (index & 7);
}
