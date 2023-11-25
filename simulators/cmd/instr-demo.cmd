@echo off
setlocal enabledelayedexpansion

REM make sure the programmer doesn't assume a start state
set X=%random%
set Y=%random%
set Z=%random%
set T=%random%

REM the interpreter comes back here each cycle
:looptop

REM limit ourselves to 16 bit unsigned for now
if !X! GEQ 0 if !X! LEQ 65535 (
	goto :ready
)
if !X! LSS 0 (
	set /a X += 65536
)
if !X! GTR 65535 (
	set /a X -= 65536
)
goto :looptop

REM once we get here the system state is ready for another instruction
:ready
echo X=!X!   Y=!Y!   Z=!Z!   T=!T!
set mem
set /p I=I=

REM do the instruction cycle
if /i !I! == INC (
	set /a X += 1
) else if /i !I! == LOAD (
	for %%i in (!Y!) do (
		set /a X = mem[%%i]
	)
) else if /i !I! == LSH (
	set /a "X = X << 1"
) else if /i !I! == POP (
	set /a X = Y
	set /a Y = Z
	set /a Z = T
) else if /i !I! == PUSH (
	set /a T = Z
	set /a Z = Y
	set /a Y = X
) else if /i !I! == QUIT (
	goto :EOF
) else if /i !I! == READ (

This is a very complicated instruction...

chan;		channel ID (physical address)
*(chan+0);	reader ID (physical address) and status flag(s)
*(chan+1);	writer ID (physical address) and status flag(s)
chan[0]+F;	address of writeable value word in reader task page
chan[1]+F;	address of readable value word in writer task page


			X	Y	Z	T
nop			.	cID	.	.	y is channel ID
nop			.	cID	.	.	calculate reader address from channel address
load			???	cID	.	.	load read address word into X
assert_task_match	rID/f	cID	.	.	verify it matches task ID - fault if not
push			rID/f	rID/f	cID	.	\
xor			0	rID/f	cID	.	get the reader flag
inc			1	rID/f	cID	.	|
and			f	rID/f	cID	.	/

readflagset:
	get writer word
	get writer status

	writeflagset:
		

readflagclear:


do state machine
RW
00	set reader ready flag
	write reader address word
	put the register stack back to initial conditions
	sleep
01	load writer address
	load offset to value (maybe make this 0)
	calculate address to value
	load value
	clear writer ready flag
	write reader address word
	



) else if /i !I! == RSH (
	set /a "X = X >> 1"
) else if /i !I! == STORE (
	for %%i in (!Y!) do (
		set /a mem[%%i] = X
	)
) else if /i !I! == XOR (
	set /a "X = X ^ Y"
) else (
	echo Bad instruction
)

goto :looptop
