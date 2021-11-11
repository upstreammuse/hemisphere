#ifndef BITSET_H
#define BITSET_H

#include <stdint.h>
#include <stdio.h>

typedef struct bitset BITSET;

/* starting with given start value, find the first clear bit */
size_t find_clear_bit(BITSET*, size_t start);

/* get the value of the specified bit */
uint8_t get_bit(BITSET*, size_t index);

/* initialize a bitset from existing memory
 * - buffer_size - size in bytes of the buffer
 * - size - size in bits of the bitset
 */
BITSET* make_bitset(void* buffer, size_t buffer_size, size_t size);

/* allocate and initialize a bitset, deallocate with free()
 * - size - size in bits of the bitset
 */
BITSET* new_bitset(size_t size);

/* print the bitset as a series of 0s and 1s, starting with bit 0 and finishing with bit size-1 */
void print_bitset(FILE*, BITSET*);

/* set the value of the specified bit */
void set_bit(BITSET*, size_t index);

#endif
