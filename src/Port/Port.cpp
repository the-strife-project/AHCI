#include <common.hpp>

void Port::stop() {
	// Stop
	port->cmd.start = 0;
	// Wait until command list stops
	while(port->cmd.cr)
		asm volatile("pause");
	// Stop FIS receive
	port->cmd.fre = 0;
	// Wait until FIS are no longer received
	while(port->cmd.fr)
		asm volatile("pause");
}

bool Port::start() {
	port->cmd.fre = 1;
	port->cmd.start = 1;
	port->ie = 0; // No interrupts
	port->is = ~0u; // Clear pending IRQs

	// Wait until command engine gets up
	for(size_t i=0; i<100000; ++i)
		asm volatile("pause");

	// Did it?
	bool cr = port->cmd.cr;
	bool fr = port->cmd.fr;
	if(!cr || !fr)
		std::printf("[AHCI] Could not start command engine:");
	if(!cr) std::printf(" CR");
	if(!fr) std::printf(" FR");
	if(!cr || !fr) {
		std::printf("\n");
		return false;
	}

	return true;
}

void Port::waitReady() {
	while(port->tfd.status.busy || port->tfd.status.drq)
		asm volatile("pause");
}

uint8_t Port::getSlot() {
	// Semaphore here! TODO
	uint32_t slots = port->sact | port->ci;
	for(size_t i=0; i<32; ++i)
		if((slots & 1) == 0)
			return i;
	return 0xFF;
}

bool Port::execute(uint8_t slot) {
	// Hold on...
	waitReady();
	// Go!
	port->ci |= (1 << slot);

	// Polling time. This will be changed in the future to make it async.
	while(port->ci & (1 << slot))
		asm volatile("pause");
	waitReady();

	// Transfer is finished
	if(port->tfd.status.err)
		return false;
	return true;
}
