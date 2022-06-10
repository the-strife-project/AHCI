#ifndef HBA_HPP
#define HBA_HPP

#include <types>

namespace HBA {
	// GHC host capability
	struct GHCCap {
		uint32_t nPorts : 5;
		uint32_t dontCare : 3;
		uint32_t nSlots : 5;
		uint32_t dontCare2 : 18;
		uint32_t longAddressing : 1;
	} __attribute__((packed));

	// Meme name, I know. Generic Host Control's Global HBA Control
	struct GHCGHC {
		uint32_t reset : 1;
		uint32_t irqEnable : 1;
		uint32_t revertSingleMessage : 1;
		uint32_t reserved : 28;
		uint32_t enable : 1;
	} __attribute__((packed));

	// Generic Host Control, first field of HBA Registers
	struct GHC {
		GHCCap cap; // Host capability
		GHCGHC ghc; // Global HBA control
		uint32_t is; // Interrupt status
		uint32_t pi; // Port implemented
		uint32_t vs; // Version
		uint32_t ccc_ctl; // Command completion coalescing control
		uint32_t ccc_pts; // Command completion coalescing ports
		uint32_t em_loc; // Enclosure management location
		uint32_t em_ctl; // Enclosure management control
		uint32_t cap2; // Host capabilities extended
		uint32_t bohc; // BIOS/OS handoff control and status
	} __attribute__((packed));

	struct SSTS {
		uint32_t det : 4; // Device Detection
		uint32_t spd : 4; // Current Interface Speed
		uint32_t ipm : 4; // Interface Power Management
		uint32_t reserved : 20;
	} __attribute__((packed));

	enum {
		SSTS_DET_NO_DEVICE,
		SSTS_DET_NO_PHY,
		SSTS_DET_UNDEFINED, // 0x2 is not defined
		SSTS_DET_OK,
		SSTS_DET_PHY_OFFLINE
	};

	#define SSTS_IPM_ACTIVE 1

	struct Command {
		uint32_t start : 1;
		uint32_t dontcare0 : 3;
		uint32_t fre : 1; // FIS Receive Enable
		uint32_t reserved : 3;
		uint32_t ccs : 5; // Current Command Slot
		uint32_t mpps : 1;
		uint32_t fr : 1; // FIS Receive Running
		uint32_t cr : 1; // Command List Running
		uint32_t dontcare1 : 16;
	} __attribute__((packed));

	// Task File Data Status
	struct TFDStatus {
		uint8_t err : 1; // Error during transfer
		uint8_t cs1 : 2; // Command specific
		uint8_t drq : 1; // Data transfer is requested
		uint8_t cs2 : 3; // Command specific
		uint8_t busy : 1;
	} __attribute__((packed));

	// Task File Data
	struct TFD {
		TFDStatus status;
		uint8_t error;
		uint16_t reserved;
	} __attribute__((packed));

	struct Port {
		uint64_t clb; // Command List Base Address (two dwords in 64 bit mode)
		uint64_t fb; // FIS Base Address (same as above)
		uint32_t is; // Interrupt Status
		uint32_t ie; // Interrupt Enable
		Command cmd; // Command and Status
		uint32_t reserved;
		TFD tfd; // Task File Data
		uint32_t sig; // Signature

		SSTS ssts; // SATA Status
		uint32_t sctl; // SATA Control
		uint32_t serr; // SATA Error
		uint32_t sact; // SATA active

		uint32_t ci; // Command Issue
		uint32_t sntf; // SATA Notification
		uint32_t fbs; // FIS-based Switching Control
		uint32_t devslp; // Device Sleep
		uint8_t reserved1[40];
		uint8_t vs[16]; // Vendor Specific
	} __attribute__((packed));

	#define PORT_SIGNATURE_SATA 0x101
	#define PORT_SIGNATURE_ATAPI 0xeb140101
	#define PORT_SIGNATURE_SEMB 0xc33C0101
	#define PORT_SIGNATURE_PM 0x96690101

	struct Registers {
		GHC ghc;
		uint8_t reserved[116];
		uint8_t vendorSpecific[96];
		Port ports[32];
	} __attribute__((packed));
}

#endif
