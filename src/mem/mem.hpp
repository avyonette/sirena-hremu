#pragma once
#include <cstring>
#include <types.h>
#include <arch_types.h>

namespace hremu::mem {
	struct ReadOnlyMemory {
		byte* ptr;
		usize size;

		ReadOnlyMemory(usize size) {
			this->ptr = new byte[size];
			this->size = size;

			std::memset(this->ptr, 0, this->size);
		}

		~ReadOnlyMemory() {
			delete[] this->ptr;
		}

		void init(byte* src, usize n) {
			std::memcpy(this->ptr, src, n);
		}

		const byte& operator[](p_addr address) const;

		byte read8(p_addr address) const;
		half_word read16(p_addr address) const;
		word read32(p_addr address) const;
	};

	struct ReadWriteMemory : public ReadOnlyMemory {
		ReadWriteMemory(usize size) : ReadOnlyMemory(size) {}

		byte& operator[](p_addr address);

		void write8(p_addr address, byte data);
		void write16(p_addr address, half_word data);
		void write32(p_addr address, word data);
	};
}
