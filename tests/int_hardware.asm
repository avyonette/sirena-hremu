#include "sdk/sierna.asm"

;jr .start

.start:
	; load BLIP_ROM_BASE into r0
	lui r0, 0xFFA0
	lli r0, 0x0000
	; add IRH_IOP_RF to r0
	addi r0, IRH_IOP_RF

	; load SYSTEM_RAM_BASE into r1
	lui r1, 0xFFFE
	lli r1, 0x0000

	; copy IRH_IOP_RF from ROM to SYSTEM_RAM_BASE (32 words)
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
	; try to write into a read-only register
	lvi rz, 1
	jr .loop

; rh`32, rfp`32, --`31, en`1
IRA:
#d32 0x00, 0x00, 0x00 ; reserved
#d32 0x00, 0x00, 0x00 ; #UDO
#d32 0x00, 0x00, 0x00 ; #DBG
#d32 0x00, 0x00, 0x00 ; #BRK
#d $le((BLIP_ROM_BASE + IRH_IOP)`32 @ (SYSTEM_RAM_BASE)`32 @ 0x0`31 @ 1`1) ; #IOP
#d32 0x00, 0x00, 0x00 ; #DBZ
#d32 0x00, 0x00, 0x00 ; #PLE
#d32 0x00, 0x00, 0x00 ; #MAE
#d32 0x00, 0x00, 0x00 ; #GPE
#d 0`23712 ; (256 total entries - 9) * 96 bits per entry = 23712 bits

IRH_IOP:
	addi r9, 1
	reti

IRH_IOP_RF: ; zeroed register frame for the IRH (128 bytes * 8 = 1024 bits total)
#d 0`1024
