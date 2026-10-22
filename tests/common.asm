; r8 - source address
; r9 - destination address
; r11 - size in words
_copy_struct32:
	lvi r10, 0 ; r10 = 0 (index start)
.copy_loop_begin:
	cmpu r12, r10, r11
	jrge .copy_loop_end, r12 ; if(r10 >= r11) goto copy_loop_end;

	mcw r9, r8 ; *r9 = *r8
	addi r8, 4 ; r0 += 4 (word size)
	addi r9, 4 ; r1 += 4

	addi r10, 1 ; r10++
	jr .copy_loop_begin
.copy_loop_end:
	ret
