#include <cstdio>
#include <userspace/PCI.hpp>
#include "common.hpp"

static std::PID pci = 0;

// Initialize an AHCI controller
static void initHBA(uint32_t device) {
	uint32_t abarPhys = std::rpc(pci,
								 std::PCI::GET_BAR,
								 device,
								 AHCI_PCI_ABAR_BAR);

	// Memory mapped?
	if(abarPhys & 1) {
		std::printf("[AHCI] Unsupported non-memory mapped I/O\n");
		return;
	}
	abarPhys &= ~0b1111;

	// Become bus master and enable MMIO
	std::rpc(pci, std::PCI::BECOME_BUSMASTER, device);
	std::rpc(pci, std::PCI::ENABLE_MMIO, device);

	// Map memory register
	uint64_t abar = std::mapPhys(abarPhys,
								 2, // Two pages
								 std::MAP_PHYS_WRITE,
								 std::MAP_PHYS_DONT_CACHE);
	if(!abar) {
		std::printf("[AHCI] No space left on address space\n");
		std::exit(2);
	}

	// Get registers
	HBA::Registers* regs = (HBA::Registers*)abar;
	if(!(regs->ghc.cap.longAddressing)) {
		std::printf("[AHCI] No support for 64 bit addressing\n");
		return;
	}

	// Enable AHCI now! :)
	regs->ghc.ghc.irqEnable = 0;
	regs->ghc.ghc.enable = 1;
	// Clear pending IRQs
	regs->ghc.is = ~0u;

	// Probe all ports
	for(size_t i=0; i<MAX_PORTS; ++i) {
		if(!(regs->ghc.pi & (1 << i)))
			continue; // Port not implemented

		HBA::Port* port = &(regs->ports[i]);

		// Check status' device detection
		switch(port->ssts.det) {
		case HBA::SSTS_DET_NO_DEVICE:
			continue; // No device
		case HBA::SSTS_DET_OK:
			break; // Very nice
		default:
			std::printf("[AHCI] Weird SSTS det (%d). Skipping.\n", port->ssts.det);
			continue;
		}

		// Check status' power mode
		if(port->ssts.ipm != SSTS_IPM_ACTIVE) {
			std::printf("[AHCI] Inactive drive! (TODO?)\n");
			continue;
		}

		DevicePort dp(device, i, port);
		switch(port->sig) {
		case PORT_SIGNATURE_ATAPI:
			// CD-ROM
			atapis.add(dp);
			break;
		}
	}
}

void probe() {
	pci = std::resolve("PCI");
	if(!pci)
		std::exit(1);

	// Ask PCI for AHCI controllers (HBAs)
	for(size_t idx=0;; ++idx) {
		uint32_t device = std::rpc(pci,
								   std::PCI::GET_DEVICE,
								   AHCI_PCI_CLASS,
								   AHCI_PCI_SUBCLASS,
								   idx);

		if(device == std::PCI::BAD_DEVICE)
			break; // And that's all

		// Got one
		initHBA(device);
	}
}
