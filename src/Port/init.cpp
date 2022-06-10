#include <common.hpp>
#include <cstdio>

bool Port::init() {
	// First, stop the command engine
	stop();

	// All structures in one physical page
	uint64_t virt = (uint64_t)std::mmap();
	phys = std::getPhys(virt);
	region = (PortStructs::PhysRegion*)virt;

	// CLB (Command List Base Address)
	port->clb = phys + 0; // cmdList offset

	// FB (FIS Base Address)
	port->fb = phys + sizeof(PortStructs::CommandList); // rfis offset

	// Calculate the command table offset (for command headers)
	cmdTableOff = phys;
	cmdTableOff += sizeof(PortStructs::CommandList);
	cmdTableOff += sizeof(PortStructs::RFIS); // cmdTable offset

	// Start again the command engine
	return start();
}
