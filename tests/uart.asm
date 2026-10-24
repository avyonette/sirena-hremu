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
.setup_uart:
	inti 0x02 ; #DBG

	; set r0 to mapped UART 0 address
	lui r0, 0xA000
	lli r0, 0x0000

	; configure packet
	lui r1, 0x0000 ; parity none
	lli r1, 9600`16 ; baud rate
	stw r0, r1

	; +4 to r0 for TX
	addi r0, 4
.loop: ; send 'HELLO' in a loop
	lvi r1, "H"
	stw r0, r1
	
	lvi r1, "E"
	stw r0, r1

	lvi r1, "L"
	stw r0, r1

	lvi r1, "L"
	stw r0, r1

	lvi r1, "O"
	stw r0, r1

	lvi r1, "\n"
	stw r0, r1

	jr .loop

#include "common.asm"

; lm`20, bs`20, --`8, idx`8, tg`8
L1_PMM:
#d $le((0xA0001000 >> 12)`20 @ (0xA0000000 >> 12)`20 @ 0`8 @ 0`8 @ 0x02`8) ; Entry 0 (0xA0000000 maps to UART 0)
#d 0`4032 ; Entry 1-63
