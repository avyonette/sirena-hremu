#pragma once
#include <arch_types.h>

#pragma pack(push, 1)
typedef struct RegisterFrame {
	word r0;
	word r1;
	word r2;
	word r3;
	word r4;
	word r5;
	word r6;
	word r7;
	word r8;
	word r9;
	word r10;
	word r11;
	word r12;
	word r13;
	word r14;
	word r15;

	word rip;
	word rtp;
	word rsp;
	word rlfp;
	word rpdp;
	word rcf;
	word rpl;

	word _reserved_0;
	word _reserved_1;
	word _reserved_2;
	word _reserved_3;
	word _reserved_4;

	word rfi0;
	word rfi1;
	word rfo0;
	word rfo1;
} RegisterFrame;
#pragma pack(pop)
