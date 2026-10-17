#include "alu.h"
#include "arch_types.h"
#include "cpu/cu.h"
#include "cpu/reg.h"

#define SET_SZCO(sf, zf, cf, of) \
	{ \
		word rcf = cu_read_reg(REG_RCF); \
		rcf = (rcf & 0xFFFFFFF0) | (of << 3) | (cf << 2) | (cf << 1) | sf; \
		cu_write_reg(REG_RCF, rcf); \
	}

#define SET_SZC(sf, zf, cf) \
	{ \
		word rcf = cu_read_reg(REG_RCF); \
		rcf = (rcf & 0xFFFFFFF8) | (cf << 2) | (cf << 1) | sf; \
		cu_write_reg(REG_RCF, rcf); \
	}

u32 alu_iadd32c(u32 a, u32 b) {
	u32 y = a + b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = y < a;
	bool of = (b > 0 && a < INT32_MIN + b) || (b < 0 && a > INT32_MAX + b);
	SET_SZCO(sf, zf, cf, of);

	return y;
}

u32 alu_isub32(u32 a, u32 b) {
	u32 y = a - b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = a < b;
	bool of = (b > 0 && a < INT32_MIN + b) || (b < 0 && a > INT32_MAX + b);
	SET_SZCO(sf, zf, cf, of);

	return y;
}

u32 alu_imul32(u32 a, u32 b) {
	u32 y = a * b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = 0;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_and32(u32 a, u32 b) {
	u32 y = a & b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = 0;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_or32(u32 a, u32 b) {
	u32 y = a | b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = 0;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_xor32(u32 a, u32 b) {
	u32 y = a ^ b;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = 0;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_lshl32(u32 a, u32 n) {
	u32 y = a << n;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = (y >> n) & 1;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_lshr32(u32 a, u32 n) {
	u32 y = a >> n;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = (y >> (31 - n)) & 1;
	SET_SZC(sf, zf, cf);

	return y;
}

u32 alu_ashr32(u32 a, u32 n) {
	// C does arithmetic shifts on signed values
	i32 y = (i32) a >> (i32) n;

	bool sf = y >> 31;
	bool zf = y == 0;
	bool cf = (y >> (31 - n)) & 1;
	SET_SZC(sf, zf, cf);

	return y;
}

// https://stackoverflow.com/a/65895611
u32 alu_signe18(u32 a) {
	return (a & 0x3FFFF) | -(a & 0x20000);
}

u32 alu_signe16(u32 a) {
	return (a & 0xFFFF) | -(a & 0x8000);
}

u32 alu_signe8(u32 a) {
	return (a & 0xFF) | -(a & 0x80);
}

/*
	u16 and u8 values represented as u32 are already zero-extended,
	but might as well make sure as the spec says "Zero-extend from bit X to 31"
	so input a might not necessarily be a u8 or u16
*/
u32 alu_zeroe16(u32 a) {
	return a & 0xFFFF;
}

u32 alu_zeroe8(u32 a) {
	return a & 0xFF;
}
