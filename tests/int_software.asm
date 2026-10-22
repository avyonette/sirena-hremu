#include "sdk/sierna.asm"

;jr .start

.start:
	; load BLIP_ROM_BASE into r0
	lui r0, 0xFFA0
	lli r0, 0x0000
	; add IRH_20_RF to r0
	addi r0, IRH_20_RF

	; load SYSTEM_RAM_BASE into r1
	lui r1, 0xFFFE
	lli r1, 0x0000

	; copy IRH_20_RF from ROM to SYSTEM_RAM_BASE (32 words)
	lvi r3, 0 ; r3 = 0 (index)
	lvi r4, 32 ; r4 = 32 (limit)
.rf_copy_loop_begin:
	cmpu r5, r3, r4
	jrge .rf_copy_loop_end, r5 ; if(r3 >= r4) goto rf_copy_loop_end;

	mcw r1, r0 ; *r1 = *r0
	addi r0, 4 ; r0 += 4 (word size)
	addi r1, 4 ; r1 += 4

	addi r3, 1 ; r3++
	jr .rf_copy_loop_begin
.rf_copy_loop_end:

	; load BLIP_ROM_BASE into r0
	lui r0, 0xFFA0
	lli r0, 0x0000
	; add IRA to r0
	addi r0, IRA

	; set rrap to IRA
	mov rrap, r0
.loop:
	; call INT 0x20
	lvi rfi0, 10
	inti 0x20
	jr .loop

; rh`32, rfp`32, --`31, en`1
IRA:
#d 0`864 ; 9 built-in interrupts
#d 0`2208 ; + 23 reserved
#d $le((BLIP_ROM_BASE + IRH_20)`32 @ (SYSTEM_RAM_BASE)`32 @ 0x0`31 @ 1`1) ; INT 0x20
#d 0`21408 ; 223 remaining

IRH_20:
	add r9, r9, rfi0
	reti

IRH_20_RF: ; zeroed register frame for the IRH (128 bytes * 8 = 1024 bits total)
#d 0`1024
