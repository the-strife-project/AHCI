#include <cstdio>
#include "common.hpp"

/*
	This AHCI driver does DMA polling instead of via IRQs.
	It's slower, yes, but I wanted to have something that worked.
	MSI is implemented in the PCI driver, I might do the switch in the near future.
*/

std::unordered_set<DevicePort> atapis;

extern "C" void _start() {
	// Find devices and initialize them
	probe();
	for(auto& x : atapis)
		x.init();

	exportProcedures();
	std::enableRPC();
	std::publish("AHCI");
	std::halt();
}
