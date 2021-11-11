#include "data_mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO the allocation scheme here is perhaps too complex and to no benefit, since data pages allocate and deallocate mostly randomly(?), but this scheme should be a good way to do stack pages, which may allocate and deallocate in batches(?) */

/* 64KB data memory */
uint8_t data_mem_array[256*256];

/* bitmap that indicates allocated pages */
/* TODO replace with bitset class to conserve memory */
uint8_t used_pages[256];

/* efficiently indicate the next free page
 * when this is overflows back to 0, we look at the bitmap instead
 * on first use, the bitmap is also efficient, so we start cleanly
 */
uint8_t next_free_page;

struct page_table_entry {
	uint16_t task;
	uint8_t logical;
	uint8_t physical;
};

struct page_table_entry page_table[256];

/* initialize data mem as at bootup */
void init_data_mem() {
	memset(used_pages, 0, 256);
	next_free_page = 0;
}

uint8_t allocate_page(uint16_t task, uint8_t logical) {
	/* if we are just starting, or if we have wrapped into bitmap mode, then find the next free page using the bitmap */
	if (!next_free_page) {
		for (int i = 0; i < 256; i++) {
			if (!used_pages[i]) next_free_page = i;
		}
	}

	/* at this point, we are either in linear mode, and the bitmap should indicate a free page, or we found a free page above using the bitmap, so the bitmap should indicate a free page */
	if (used_pages[next_free_page]) {
		fprintf(stderr, "data pages exhausted\n");
		/* TODO go into a debug halt state? */
		exit(EXIT_FAILURE);
	}

	/* allocate the page into the page table */
	used_pages[next_free_page] = 1;
	for (int i = 0; i < 256; i++) {
		if (!page_table[i].task) {
			page_table[i].task = task;
			page_table[i].logical = logical;
			page_table[i].physical = next_free_page;
			break;
		}
	}
	/* TODO would like a way to assert that the page table had a free entry, but currently don't have that.  maybe an allocation scheme like the pages themselves?  too heavy? */

	/* if the next page isn't free, then we aren't in linear mode, so reset to bitmap mode */
	next_free_page++;
	if (used_pages[next_free_page]) {
		next_free_page = 0;
	}
}

uint8_t read_data_mem(uint16_t addr) {
	return data_mem_array[addr];
}

void write_data_mem(uint16_t addr, uint8_t data) {
	data_mem_array[addr] = data;
}
