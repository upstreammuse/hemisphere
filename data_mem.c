#include "data_mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitset.h"

/* TODO the allocation scheme here is perhaps too complex and to no benefit, since data pages allocate and deallocate mostly randomly(?), but this scheme should be a good way to do stack pages, which may allocate and deallocate in batches(?) */

#define PAGE_COUNT 256
#define PAGE_COUNT_TYPE uint8_t
#define PAGE_SIZE 256
#define TASK_COUNT_TYPE uint16_t

/* data page memory */
uint8_t data_mem_array[PAGE_COUNT * PAGE_SIZE];

/* bitmap that indicates allocated pages */
uint8_t used_pages_backing[PAGE_COUNT >> 3];
BITSET* used_pages;

/* efficiently indicate the next free page
 * when this is overflows back to 0, we look at the bitmap instead
 * on first use, the bitmap is also efficient, so we start cleanly
 */
PAGE_COUNT_TYPE next_free_page;

struct page_table_entry {
	TASK_COUNT_TYPE task;
	PAGE_COUNT_TYPE logical;
	PAGE_COUNT_TYPE physical;
};

/* TODO consider making the page table occupy some of the pages and be managed by an "OS" routine that hooks into the normal data read/write instructions, coupled with an "OS level" instruction to read/write raw memory addresses without checking */
struct page_table_entry page_table[PAGE_COUNT];

/* initialize data mem as at bootup */
void init_data_mem() {
	used_pages = make_bitset(used_pages_backing, sizeof (used_pages_backing), PAGE_COUNT);
	next_free_page = 0;
}

PAGE_COUNT_TYPE allocate_page(TASK_COUNT_TYPE task, PAGE_COUNT_TYPE logical) {
	/* if we are just starting, or if we have wrapped into bitmap mode, then find the next free page using the bitmap */
	if (!next_free_page) {
		next_free_page = find_clear_bit(used_pages, 0);
	}

	/* if we don't have a free page, we are broken */
	/* TODO go into a debug halt state? */
	if (get_bit(used_pages, next_free_page)) {
		fprintf(stderr, "data pages exhausted\n");
		exit(EXIT_FAILURE);
	}

	/* allocate the page into the page table */
	/* TODO would like a way to assert that the page table had a free entry, but currently don't have that, and relying on deallocation logic to keep it in sync with allocated pages */
	set_bit(used_pages, next_free_page);
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (!page_table[i].task) {
			page_table[i].task = task;
			page_table[i].logical = logical;
			page_table[i].physical = next_free_page;
			break;
		}
	}

	/* if the next page isn't free, then we aren't in linear mode, so reset to bitmap mode */
	/* if we just allocated the last page in linear mode, then we have entered bitmap mode */
	next_free_page++;
	if (get_bit(used_pages, next_free_page)) {
		next_free_page = 0;
	}
}

uint8_t read_data_mem(uint16_t addr) {
	return data_mem_array[addr];
}

void write_data_mem(uint16_t addr, uint8_t data) {
	data_mem_array[addr] = data;
}
