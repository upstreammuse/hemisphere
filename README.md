Hemisphere is a CPU design that supports safe concurrent programming.



# Notes and Musings

## Task switching overhead

It is desirable to minimize the amount of shuffling that must take place between registers and workspace memory during a task switch.  (Otherwise there's not much point to hardware-assisted concurrency.)  The transputer had fixed places to yield, so that user code would be able to make guarantees about the state of the registers before and after a yield.  This is perhaps worth considering.

## Shadowing the top of the stack in registers

There are complexities with treating XYZT as an integral part of the expression stack vs a separate entity.  For example, say we are just starting and X=3, Y=17, Z=0, T=0.  Then we push.  Does the memory stack take on a 0 value from T?  How do we know if that 0 value is an actual value or uninitialized register space?

It might be possible to 'optimize' by not not pushing 0s as long as the stack is empty (they need to be pushed if there are already values in the stack, so that the pops can return the correct number of 0s before the nonzero value).  In this manner, popping the empty stack into T sets T to zero.

Conceptually this means the stack has an infinite number of zeros preceding the first non-zero value.

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
