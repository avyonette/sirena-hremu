#include "alu.h"

// Source - https://stackoverflow.com/a/31655073
// Posted by Kevin Thibedeau
// Retrieved 2026-10-17, License - CC BY-SA 3.0
#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

u32 sext17(u32 a) {
	return SIGNEX(a, 16);
}

u32 sext16(u32 a) {
	return SIGNEX(a, 15);
}

u32 sext8(u32 a) {
	return SIGNEX(a, 7);
}

/*
	u16 and u8 values represented as u32 are already zero-extended,
	but might as well make sure as the spec says "Zero-extend from bit X to 31"
	so input a might not necessarily be a u8 or u16
*/
u32 zext16(u32 a) {
	return a & 0xFFFF;
}

u32 zext8(u32 a) {
	return a & 0xFF;
}
