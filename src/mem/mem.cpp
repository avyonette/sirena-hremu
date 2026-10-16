#include "mem.hpp"

namespace hremu::mem {
	const byte& ReadOnlyMemory::operator[](p_addr address) const {
		assert(address < this->size);
		return this->ptr[address];
	}

	byte ReadOnlyMemory::read8(p_addr address) const {
		assert(address < this->size);
		return this->ptr[address];
	}

	half_word ReadOnlyMemory::read16(p_addr address) const {
		assert(address + 1 < this->size);
		
		half_word data = 0;
		data |= this->read8(address);
		data |= this->read8(address + 1) << 8;

		return data;
	}

	word ReadOnlyMemory::read32(p_addr address) const {
		assert(address + 3 < this->size);
		
		word data = 0;
		data |= this->read8(address);
		data |= this->read8(address + 1) << 8;
		data |= this->read8(address + 2) << 16;
		data |= this->read8(address + 3) << 24;

		return data;
	}

	byte& ReadWriteMemory::operator[](p_addr address) {
		assert(address < this->size);
		return this->ptr[address];
	}

	void ReadWriteMemory::write8(p_addr address, byte data) {
		assert(address < this->size);
		this->ptr[address] = data;
	}

	void ReadWriteMemory::write16(p_addr address, half_word data) {
		assert(address + 1 < this->size);

		this->ptr[address] = data & 0xFF;
		this->ptr[address + 1] = data >> 8;
	}

	void ReadWriteMemory::write32(p_addr address, word data) {
		assert(address + 3 < this->size);

		this->ptr[address] = data & 0xFF;
		this->ptr[address + 1] = (data >> 8) & 0xFF;
		this->ptr[address + 2] = (data >> 16) & 0xFF;
		this->ptr[address + 3] = (data >> 24) & 0xFF;
	}
}
