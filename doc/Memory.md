This document discusses the overall memory architecture.  Ideally this is a secondary concern to questions of tasking layout, context switching, and other performance measures, but some level of basic groundwork needs to exist to enable useful discussions of those other matters.

# Overall Layout

On the side of raw performance, a segmented architecture using a base and bounds pair is so far the best identified method.  However, because of fragmentation, pure segmentation suffers especially in contexts with many tasks that come and go.  Were it possible to have a segmented architecture without the pitfalls of fragmentation, it would be a great mix of speed and simplicity.

A paged architecture is most familiar to the memory models of other CPUs outside the microcontroller and embedded design space.  Page size becomes a set of tradeoffs between overall memory capacity, performance degradation due to page walks, the size of the MMU's TLB, the overhead of page tables compared to usable space, etc.

# Paging and Word Size

So far a 16-bit architecture seems the minimally viable word size to do anything with for this project.  The limitations of 8 bit were too readily apparent in early designs, wherein the need to constantly stack values for memory access made for more expensive context switches, and made paging more difficult.  For completeness's sake, here is an example 8-bit architecture.

Assuming the use of a commodity through-hole SRAM with 4 Mb capacity, a 19 bit address, and 8 bit data path.  A page table entry could supply at most 7 bits of page address, keeping a single bit for a "page present" flag, and no other protection bits.  This leaves 12 bits of page offset, which is too big.  To work around this, separate the overall 19-bit address space into banks such that each page context exists within a given bank.  Logical addresses are 16 bits, with the first 8 bits providing a 7-bit page address... that doesn't work.  You can't map from 256 logical to 128 physical!  So logical addresses are 15 bits, and the top 7 index half the capacity of a 256 word page?  That's wasteful.  So logical addresses are 14 bits, pages are 128 words, and the top 7 bits index a page that translates into 7 bit page addresses and that combine with the 7 bits of logical offset.  This gives us 32 global contexts that pages exist within, which are completely separate from each other.  As part of the task switching machinery, the CPU could increment its way through those 32 contexts, which each maintain an independent set of tasking structures, free list, etc.  There is basically no way to pass anything between contexts without the help of a special addressing mode or some passthrough register.  And of course all addresses are stacked into at least 2 pieces.  It's a lot of extra complexity for the sake of narrowing the internal busses.

A 16-bit architecture simplifies things and works like so.  The largest possible page size is the entire 16 bit address space, leaving 8 pages in the system, which is silly.  The smallest possible page size is 4 bits, since a 16-bit page table entry can hold at most 15 bits of page address with no other protection bits.  It makes sense to allow at least one permission bit and state the minimum page size is 5 bits.  Within these boundaries, we can explore optimal page sizes.

| Page Size (bits)	| Page Table Depth	| Maximum Address Space (bits)	| Delta from Ideal
| ---                | ---                | ---                            | ---
| 5			         | 0			         | 5				                  | `***********` 11
|                    | 1			         | 10				                  | `******` 6
|                    | 2			         | 15				                  | `*` 1
| 6			         | 0			         | 6				                  | `**********` 10
|                    | 1		      	   | 12				                  | `****` 4
|                    | 2			         | 18				                  | `**` 2
| 7			         | 0			         | 7				                  | `*********` 9
|                    | 1			         | 14				                  | `**` 2
|			            | 2			         | 21				                  | `*****` 5
| 8			         | 0			         | 8				                  | `********` 8
|			            | 1			         | 16				                  | 0
| 9			         | 0			         | 9				                  | `*******` 7
|			            | 1			         | 18				                  | `**` 2
| 10			         | 0			         | 10				                  | `******` 6
|			            | 1			         | 20				                  | `****` 4
| 11			         | 0			         | 11				                  | `*****` 5
|			            | 1			         | 22				                  | `******` 6
| 12			         | 0			         | 12				                  | `****` 4
|			            | 1			         | 24				                  | `********` 8
| 13			         | 0			         | 13				                  | `***` 3
|			            | 1			         | 26				                  | `**********` 10
| 14			         | 0			         | 14				                  | `**` 2
|			            | 1			         | 28				                  | `************` 12
| 15			         | 0			         | 15				                  | `*` 1
|			            | 1			         | 30				                  | `**************` 14
| 16			         | 0			         | 16				                  | 0

Eliminating combinations with a delta greater than 2 leaves us with.

| Page Size (bits)	| Page Table Depth	| Maximum Address Space (bits)	| Delta from Ideal
|---                 |---                 |---                             |---
| 5			         | 2			         | 15				                  | `*` 1
| 6			         | 2			         | 18				                  | `**` 2
| 7			         | 1			         | 14				                  | `**` 2
| 8			         | 1			         | 16				                  | 0
| 9			         | 1			         | 18				                  | `**` 2
| 14			         | 0			         | 14				                  | `**` 2
| 15			         | 0			         | 15				                  | `*` 1
| 16			         | 0			         | 16				                  | 0

Given the intended use case of many tasks, it's worth also considering the total number of pages available, and how much overhead is consumed in paging for a typical task.  With 5 bit pages, there are 2^14 pages in the system, but a single task will use a page for the task, up to 33 pages of stack page table, up to 33 pages for a data segment.  67 page each 5 bits is 2144 words.  With 8 bits pages, there are 2^11 pages in the system, and a single task will use a page for the task, 1 page for stack page table, and 1 page for a data segment.  3 pages each 8 bits is 768 words.  However, The smallest possible overhead for a task with 5-bit pages is only 5 pages or 160 words, while the smallest possible overhead for a task with 8-bit pages remains 768 words.

Put another way, with 2^14 total pages, a task needs at least 7 pages to do anything, giving a max of 2340 tasks.  With 2^11 total pages, a task needs at least 5 pages, giving a max of 409 tasks.

With 5 bit pages, the overhead is 33 pages for a 15-bit address space, or about 3%.  With 8 bit pages, the overhead is 1 page for a 16 bit address space, or about .4%.  Consider if a task uses, say 10 bits of address space.  With 5 bit pages, that's 2 pages or 64 words.  With 8 bit pages, it's the one page of 256 words.

Basically, if the tasks are small enough, the 5 bit pages can provide space savings, and can provide more tasks of minimum size, but are going to be less efficient than 8 bit pages for larger tasks.

Once the word size gets wide enough, it becomes possible to pack multiple page addresses in a single word, increasing the number of pages a single page of page table can provide access to.  For example, a 32 bit machine could contain 2 of the above entries in each word of a page, so a 32-word page would provide access to 64 pages.

# Instructions in Memory

A general premise was to have small fixed-width instructions, using the stack to provide arguments to instructions, and keeping the individual instruction execution aligned with RISC basics.  A basic assumption has been that 8-bit instructions would be enough, so that even in a 16 bit system there could fit two instructions per word, so that instruction fetch would not dominate the memory bus.

When doing this, the lowest bit(s) of the program counter become an offset into the instruction register to find the correct instruction, and the remaining upper bits of the program counter become the memory address to fetch from.  This imposes the practical limitation that there should be a power of 2 instructions per word, so that all possible lowest bit values translate into a valid location within the instruction register.  For example, packing 3 8-bit instructions into a 24-bit word would not work well, because every four program counter values would result in an invalid instruction location.

# Segments

Even though paging is needed to prevent fragmentation, there's no need to limit a task to a single address space.  By giving tasks access to multiple segments of address space, applications can isolate parts of their memory from each other, allowing portions of code access to only some memory, protecting other memory areas from access.  One way to do this is to have system-level pointers or handles to these memory segments, held in the stack, and using the stack's own memory protection mechanisms to prevent access to segments whose handles are not within the visible stack space.

In order to make this really work, segment handles need to be values that cannot be otherwise created by a task.  In other words, a segment handle cannot be created from a raw integer address.  It's analogous to preventing typecasting at the hardware level.

# Data Tagging

One way to enable special values such as segment handles is by tagging words of memory, giving each word a data type.  Types assigned in this way are fundamental to the hardware, so it becomes inexpressibly impossible for a task to access memory that is not otherwise accesible.

This can be extended within some reason to have different data types in hardware and to prevent the use of invalid data types for certain instructions.  In this architecture, the primary goal right now is to prevent task code from generating handle addresses and similar data types that can be used to access arbitrary memory outside the allowed segments of the task.

This leads to the conundrum of how the system itself manages such special data types.  At some level, either an operating system-like application must be able to perform conversions in order to allocate memory, build page tables, etc., or the hardware must provide a complete set of facilities for doing the same.  Building an operating system into a processor seems like bad abstraction.  Otherwise, certain instructions may require special permissions that are only granted to system-level tasks with these functions.
