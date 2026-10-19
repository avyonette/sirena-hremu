; this program finishes in ~75 cycles
; results:
; r1 = CD35400C
; r2 = CCD64B98
; r10 = 9A0B8BA4

#include "sdk/sierna.asm"

jr start

#align 32
data0:
#d16 $le(97`16)
#d16 $le(3`16)

#align 32
data1:
#d16 $le(115`16)
#d16 $le(12`16)

start:
	; load BLIP_ROM_BASE into r0
	lui r0, 0xFFA0
	lli r0, 0x0000
	; add data0 to r0
	addi r0, data0 ; r0 = BLIP_ROM_BASE + data0

	; load first 16 bytes of data0 to r10
	ldhz r10, r0
	; load last 16 bytes of data0 to r11
	addi r0, 2
	ldhz r11, r0

	; load first 16 bytes of data1 to r12
	addi r0, 2
	ldhz r12, r0
	; load last 16 bytes of data1 to r13
	addi r0, 2
	ldhz r13, r0

	; loop until an overflow occurs
.loop:
	mul r1, r10, r11 ; r1 = r10 * r11
	sub r2, r10, r13 ; r2 = r10 - r13
	add r10, r1, r2 ; r10 = r1 + r2

	; check carry flag
	lvi r6, 0b0100 ; r6 = 0b1000
	mov r5, rcf ; r5 = rcf
	and r7, r5, r6 ; r7 = r5 & r6
	cmpu r8, r7, rz ; r8 = cmp(r7, rz)
	jre .loop, r8 ; jump if carry flag not set (=0)
.end:
	jr .end ; loop forever
