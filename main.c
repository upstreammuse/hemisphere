#include <assert.h>
#include <stdlib.h>
#include "bitset.h"
#include "data_mem.h"
#include "hmath.h"

double math_func(double x) {
	return x * x - 3.2;
}

int main() {
	double x = 3.141587894386718927341324123;
	printf("%.20lf\n", sqrt(x));
	printf("attempting to solve an equation\n");
	printf("%.20lf\n", nsolve(math_func, 0, 10));

	BITSET* b = new_bitset(13);
	set_bit(b, 12);
	set_bit(b, 3);
	set_bit(b, 6);
	set_bit(b, 7);
	set_bit(b, 0);
	assert(find_clear_bit(b, 12) == (size_t)-1);
	assert(find_clear_bit(b, 6) == 8);
	assert(find_clear_bit(b, 1) == 1);
	print_bitset(stdout, b);
	printf("\n");
	free(b);

	/*
	assembly program may want to say [AF01] = C3
	which is write a byte to a 16 bit address

	at the chip level, this means
	- allocate page to logical address AF (if not already)
	- provide physical page translation AF->HH
	- configure address bus to HH01
	- configure data bus to C3
	- clock data into memory

	given stack-based nature of the machine, what does asm look like?
	...instructions to generate AF in X...
	select_page
	...instructions to generate 01 in X...
	push
	...instructions to generate C3 in X...
	write_data

	consider writing an array of data
	// configure address of first write
	// push data for first write
	// 'write 1'
	//  .. desire to drop pushed value but keep address for increment
	// increment address
	// push valid for second write
	// 'write 2'
	// etc.

	vs writing a single value, which is 'write 1' plus 'drop 2', and can be synthesized as such if we run out of instruction space
	*/

	write_data_mem(45, 0x87AF, 0x64);

	return 0;
}
