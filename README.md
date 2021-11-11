# About

Hemisphere is a virtual CPU that supports safe concurrent programming.



# Architecture

## Memory

The CPU makes use of multiple internal memory busses to improve data throughput.

- 256 pages x 256 bytes of data memory
- 256 pages x 256 bytes of program memory
- 1024 pages x 32 bytes of stack memory
- 1024 pages x 32 bytes of workspace memory

> Note:  The page layout of stack and workspace memory is very tentative.

## Registers

- CP - 8-bit code page selector
- PC - 8-bit program counter
- X, Y, Z, T - 8-bit values at the accessible end of an expression stack, with X
  the most accessible, then Y, with T the least
- DP - 8-bit logical data page selector, which holds the logical data page
  number in use
- DP' - 8-bit physical data page selector, which holds the physical data page
  number in use.
- S - status flags
	- RPN - If set, stack operations do not make use of the in-memory portion of
	  the expression stack.  Operations that would cause T to push its value
	  into memory cause the T value to be lost.  Operations that would cause T
	  to obtain a value from memory cause the T value to remain constant.  This
	  allows the register stack to be used in a manner similar to RPN
	  calculators.  Because the in-memory expression stack is not used, a task
	  executing with the RPN flag set does not require any expression stack
	  allocated to it.
- W - 16-bit workspace pointer.  TBD if this points to a location in workspace
  memory or if it is a task ID that is multiplied by 32(?) to obtain a workspace
  address.
- SP - 16-bit expression stack pointer.  Upper bits refer to the current logical
  stack page, and lower bits refer to the address within the stack page.  SP is
  automatically managed by instructions, so it does not have defined regions of
  upper/lower bits as with CP and DP.  Additionally, the value of SP is not
  generally useful or visible to user code, so it does not require shadowing of
  physical vs. logical pages as with DP and DP'.



# Notes and Musings

## Task switching overhead

It is desirable to minimize the amount of shuffling that must take place between
registers and workspace memory during a task switch.  (Otherwise there's not
much point to hardware-assisted concurrency.)  The transputer had fixed places
to yield, so that user code would be able to make guarantees about the state of
the registers before and after a yield.  This is perhaps worth considering.

## Stack memory paging vs segmentation and fragmentation

Stack memory is perhaps segmentable since it is not accessed via arbitrary
read/write.  Fragmentation is a concern, however, so need to determine what is
better, fragmentation waste or page waste due to fixed size allocations.

To begin with, is fragmentation solvable at all?

Task 1 starts task 2 and task 3, then continues, then joins
Task 2 starts task 4, then continues, then joins
Task 3 starts task 5, then continues, then joins
Task 3 later starts task 6, then continues, then joins

Task 1 does its thing
1111111-----2--------3---------
           ^ max stack that 1 needs via analysis (including tasks '1a' done while 2 and 3 are running)
		            ^ max stack that 2 needs via analysis (including '2a' tasks while 4 is running)
					          ^ max that 3 needs (inc. '3a' while 5 running)

Task 2 does its thing
1111111-----2--------3---------4----------

Task 3
1111111-----2--------3---------4----------5-------

Task 2 finishes
1111111-----xxxxxxxxx3---------xxxxxxxxxxx5-------
            ^ task 2 stack no longer needed
			                   ^ task 4 stack no longer needed

Task 3 joins with 5
1111111-----xxxxxxxxx3---------xxxxxxxxxxxxxxxxxxx

Task 3 starts 6
1111111-----xxxxxxxxx3---------xxxxxxxxxxxxxxxxxxx
            ^ this is perhaps too small
			                   ^ how do we figure out that we can start here?
                                                  ^ or do we end up out here?

Over time this leads to fragmentation of the stack memory.

There are space tradeoffs in that the more pages, the larger the page table, and
thus more memory to hold the page table, which may have to come out of the
overall memory budget (since the CPU is going to have some fixed amount of
onboard memory).  If stacks are segmented, then each task needs to track the
base address and size, and that can be stored as part of the workspace memory.

1024 page table entries is...
- 2 bytes for task id
- 2 bytes for logical page #
- 2 bytes for physical page #
6 \* 1024 bytes out of the possible 64K

Also it's worth noticing that if the CPU has 4 registers in RPN mode, there are
tasks that may not need any expr stack space, so it may be possible to have
stack-free tasks that mean we don't need as many stack pages as we have tasks.

## Shadowing the top of the stack in registers

There are complexities with treating XYZT as an integral part of the expression
stack vs a separate entity.  For example, say we are just starting and
X=3, Y=17, Z=0, T=0.  Then we push.  Does the memory stack take on a 0 value
from T?  How do we know if that 0 value is an actual value or uninitizlied
register space?

It might be possible to 'optimize' by not not pushing 0s as long as the stack is
empty (they need to be pushed if there are already values in the stack, so that
the pops can return the correct number of 0s before the nonzero value).  In this
manner, popping the empty stack into T sets T to zero.

Conceptually this means the stack has an infinite number of zeros preceding the
first non-zero value.

Example to follow up:
// XYZT=0
LOAD1
// X=1 YZT=0
SHIFT4
// X=16 YZT=0
INCR
// X=17 YZT=0
PUSH (T is zero and stack is empty, so no push to memory)
// X=17 Y=17 ZT=0
LOAD1
// X=1 Y=17 ZT=0
SHIFT1
// X=2 Y=17 ZT=0
INCR
// X=3 Y=17 ZT=0
PUSH (T is zero and stack is empty, so no push to memory)
// X=3 Y=3 Z=17 T=0
LOAD0
// X=0 Y=3 Z=17 T=0
PUSH (T is zero and stack is empty, so no push to memory)
// X=0 Y=0 Z=3 T=17
PUSH (T is not zero, thus into memory we go)
// X=0 Y=0 Z=0 T=3 SP=17
RPN_ON
POP
// X=0 Y=0 Z=3 T=3 SP=17
PUSH (stack is not empty, so into memory we go)
// X=0 Y=0 Z=0 T=3 SP=3,17
LOAD1
// X=1 Y=0 Z=0 T=3 SP=3,17
PUSH
// X=1 Y=1 Z=0 T=0 SP=3,3,17
PUSH
PUSH
// X=1 Y=1 Z=1 T=1 SP=0,0,3,3,17
RPN_OFF
POP
POP
// X=1 Y=1 Z=0 T=0 SP=3,3,17
POP
POP
POP
// X=0 Y=3 Z=3 T=17 SP=
POP (stack is empty, so provides infinite zeros)
// X=3 Y=3 Z=17 T=0

I like it...
