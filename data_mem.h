#ifndef DATA_MEM_H
#define DATA_MEM_H

#include <stdint.h>

/* number of data pages */
#define PAGE_COUNT 256

/* the size of a page in words */
#define PAGE_SIZE 256

/* type of an address to ensure it has enough bits */
/* number of needed bits is log2(PAGE_SIZE) + log2(PAGE_COUNT) */
#define ADDR_TYPE uint16_t

/* type of a data word */
/* NOTE:  each address results in a unique word, so there is no overlap (e.g.
 * accessing 32 bits against byte-aligned addresses) */
#define DATA_TYPE uint8_t

/* type of a page count to ensure it has enough bits */
/* number of needed bits is log2(PAGE_COUNT) */
#define PAGE_COUNT_TYPE uint8_t

/* the number of bits in an address that represent the offset within a page */
/* number of needed bits is log2(PAGE_SIZE) */
#define PAGE_SIZE_BITS 8

/* type of a task ID to ensure it has enough bits */
#define TASK_COUNT_TYPE uint16_t

/* read the word of data memory allocated to the given task and address, taking
 * into account logical to physical page translation. */
/* TODO this is currently a 'shortcut' in that it may be that the CPU ends up
 * with separate instructions for modifying DP vs reading and writing, vs. a
 * single instruction that takes a full 16-bit address. */
DATA_TYPE read_data_mem(TASK_COUNT_TYPE task, ADDR_TYPE addr);

/* write the word of data memory allocated to the given task and address, taking
 * into account logical to physical page translation. */
/* TODO this is currently a 'shortcut' in that it may be that the CPU ends up
 * with separate instructions for modifying DP vs reading and writing, vs. a
 * single instruction that takes a full 16-bit address. */
void write_data_mem(TASK_COUNT_TYPE task, ADDR_TYPE addr, DATA_TYPE data);

#endif
