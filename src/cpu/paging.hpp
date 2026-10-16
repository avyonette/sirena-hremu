#pragma once
#include <arch_types.h>
#include <types.h>

namespace hremu::cpu {
#pragma pack(push, 1)
	struct PageDirectoryEntry {
		bool p : 1;
		u8 _reserved_0 : 3 = 0;
		p_addr adr : 28;
	};

	struct PageDirectory {
		PageDirectoryEntry entries[256];
	};

	struct PageDescriptor {
		bool p : 1;
		u8 pl : 2;
		bool rw : 1;
		bool ex : 1;
		u8 _reserved_0 : 7 = 0;
		p_addr adr : 20;
	};

	struct PageTable {
		PageDescriptor entries[16];
	};
#pragma pack(pop)
}
