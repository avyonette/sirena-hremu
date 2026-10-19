#pragma once
#include <arch_types.h>

#define REG_R0 0x00
#define REG_R1 0x01
#define REG_R2 0x02
#define REG_R3 0x03
#define REG_R4 0x04
#define REG_R5 0x05
#define REG_R6 0x06
#define REG_R7 0x07
#define REG_R8 0x08
#define REG_R9 0x09
#define REG_R10 0x0A
#define REG_R11 0x0B
#define REG_R12 0x0C
#define REG_R13 0x0D
#define REG_R14 0x0E
#define REG_R15 0x0F

#define REG_RIP 0x10
#define REG_RTP 0x11
#define REG_RSP 0x12
#define REG_RLFP 0x13
#define REG_RPDP 0x14
#define REG_RCF 0x15
#define REG_RPL 0x16

#define REG_RSRV0 0x17

#define REG_RFI0 0x18
#define REG_RFI1 0x19
#define REG_RFO0 0x1A
#define REG_RFO1 0x1B

#define REG_RSRV1 0x1C

#define REG_RFP 0x1D
#define REG_RRAP 0x1E
#define REG_RZ 0x1F

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

	word rfi0;
	word rfi1;
	word rfo0;
	word rfo1;

	word _reserved_1;
	word _reserved_2;
	word _reserved_3;
	word _reserved_4;
} RegisterFrame;
#pragma pack(pop)
