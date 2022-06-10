#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

namespace PortStructs {
	// One per slot
	struct CommandHeader {
		// First dword
		uint32_t cfl : 5; // Command FIS length
		uint32_t atapi : 1;
		uint32_t write : 1;
		uint32_t prefetchable : 1;
		uint32_t reset : 1;
		uint32_t bist : 1; // BIST FIS
		uint32_t c : 1; // Clear Busy upon R_OK (??????)
		uint32_t reserved0 : 1;
		uint32_t pmp : 4; // Port Multiplier Port
		uint16_t prdtl = 0; // Number of PRDT entries

		// Second dword
		uint32_t prdbc = 0; // PRD byte count (transferred)

		// Third and fourth dwords
		// This is actually two dwords, second one only being used in 64 bit mode
		//   but this driver only supports 64 bit mode, so...
		uint64_t ctba = 0; // Command Table Descriptor Base Address (128 byte aligned)

		// Some reserved bytes
		uint32_t reserved1[4] = {0};

		CommandHeader() {
			cfl = atapi = write = prefetchable = reset = bist = c = 0;
			reserved0 = pmp = 0;
		}
	} __attribute__((packed)); // 32 bytes

	#define COMMAND_HEADERS 32
	struct CommandList {
		CommandHeader headers[COMMAND_HEADERS]; // Nothing too complex
	} __attribute__((packed)); // 1024 bytes

	// Received FIS
	struct RFIS {
		uint8_t dsfis[28]; // DMA Setup FIS
		uint8_t pad0[4];

		uint8_t psfis[20]; // PIO Setup FIS
		uint8_t pad1[12];

		uint8_t rfis[20]; // Register, Device to Host FIS
		uint8_t pad2[4];

		uint8_t sdbfis[8]; // Set Device FIS

		uint8_t ufis[64]; // Unknown FIS

		uint8_t reserved[96];
	} __attribute__((packed)); // 256 bytes

	// Physical Region Descriptor
	struct PRD {
		// Again, this is two dwords in 64 bit mode
		uint64_t dba = 0; // Data Base Address (word aligned!)
		uint32_t reserved0 = 0;

		uint32_t dbc : 22; // Byte Count (must be even!)
		uint32_t reserved1 : 9;
		uint32_t irqOnCompletion : 1;

		PRD() { dbc = reserved1 = irqOnCompletion = 0; }
	} __attribute__((packed)); // 16 bytes

	struct CommandTable {
		uint8_t cfis[64]; // Command FIS
		uint8_t acmd[16]; // ATAPI Command
		uint8_t reserved[48];
		PRD prdt[168]; // Read below
	} __attribute__((packed)); // 2816 bytes

	/*
		AHCI needs three structures per port, they are the command list,
		  the command table, and the received FIS.
		Command list is always 1024 bytes.
		Received FIS is always 256 bytes.
		Command table is 128 bytes plus 16*n, where n is the number of
		  PRDs, chosen by the OS, up to 2^16 - 1 entries.
		This means that it is indeed possible to choose a number of PRDs so that
		  all the structures of a port are in one physical page.

		Let's do the math:
		  - CommandList: 1024 bytes
		  - RFIS: 256 bytes
		  - CommandTable: 128+16*n bytes
		CommandTable must be 128-byte aligned, which it is, so that's nice.
		Now, 1024+256+(128+16*n) = 4096 <==> n = 168
		So 168 PRDT entries :)
	*/

	// Because of the above
	struct PhysRegion {
		CommandList cmdList;
		RFIS rfis;
		CommandTable cmdTable;
	} __attribute__((packed)); // 4096 bytes
	// ↑ One of those per port
}

#endif
