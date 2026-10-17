#pragma once
#include <arch_types.h>
#include <types.h>

#pragma pack(push, 1)
typedef struct PageDirectoryEntry {
	bool p : 1;
	u8 _reserved_0 : 3;
	p_addr adr : 28;
} PageDirectoryEntry;

typedef struct PageDirectory {
	PageDirectoryEntry entries[256];
} PageDirectory;

typedef struct PageDescriptor {
	bool p : 1;
	u8 pl : 2;
	bool rw : 1;
	bool ex : 1;
	u8 _reserved_0 : 7;
	p_addr adr : 20;
} PageDescriptor;

typedef struct PageTable {
	PageDescriptor entries[16];
} PageTable;
#pragma pack(pop)
