#include "sdk/sierna.asm"

.start:
.copy_l1_pmm: ; copy L1_PMM to level-1 memory map (0xFFFD E000)
	; set source address
	lui r8, 0xFFA0
	lli r8, 0x0000
	addi r8, L1_PMM

	; set destination address
	lui r9, 0xFFFD
	lli r9, 0xE000
	
	; set size in words
	lvi r11, 128

	; copy
	callr _copy_struct32
.copy_rf: ; copy IRH_20_RF to system RAM (0xFFFE 0000)
	; set source address
	lui r8, 0xFFA0
	lli r8, 0x0000
	addi r8, IRH_20_RF

	; set destination address
	lui r9, 0xFFFE
	lli r9, 0x0000

	; set size in words
	lvi r11, 32

	; copy
	callr _copy_struct32
.setup_ints:
	; load BLIP_ROM_BASE into r0
	lui r0, 0xFFA0
	lli r0, 0x0000
	; add IRA to r0
	addi r0, IRA

	; set rrap to IRA
	mov rrap, r0
.setup_pit:
	inti 0x02 ; #DBG

	; set r0 to mapped PIT 0 address
	lui r0, 0xA000
	lli r0, 0x0000

	; 0x02 Set target INT
	lui r1, 0x0000
	lli r1, (0x20`13 @ 0x02`3) ; INT 0x20
	stw r0, r1 ; write r1 to 0xA000 0000

	; 0x01 Set tick rate (in Hz)
	lli r1, (10`13 @ 0x01`3) ; 10 Hz
	stw r0, r1

	; 0x00 Set mode
	lli r1, (0x02`13 @ 0x00`3) ; Tick
	stw r0, r1

	; the timer should now be enabled and calling INT 0x20 in 1-second intervals
.end:
	jr .end

#include "common.asm"

; rh`32, rfp`32, --`31, en`1
IRA:
#d 0`864 ; 9 built-in interrupts
#d 0`2208 ; + 23 reserved
#d $le((BLIP_ROM_BASE + IRH_20)`32 @ (SYSTEM_RAM_BASE)`32 @ 0x0`31 @ 1`1) ; INT 0x20
#d 0`21408 ; 223 remaining

IRH_20:
	addi r5, 1
	reti

IRH_20_RF: ; zeroed register frame for the IRH (128 bytes * 8 = 1024 bits total)
#d 0`1024

; lm`20, bs`20, --`8, idx`8, tg`8
L1_PMM:
#d $le((0xA0001000 >> 12)`20 @ (0xA0000000 >> 12)`20 @ 0`8 @ 0`8 @ 0x01`8) ; Entry 0 (0xA0000000 maps to PIT 0)
#d 0`4032 ; Entry 1-63
