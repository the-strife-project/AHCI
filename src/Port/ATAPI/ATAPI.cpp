#include <common.hpp>

// Send ATAPI packets
bool Port::sendATAPI(ATAPI::CommandPacket* cmd, uint8_t* data, size_t len) {
	// This is an implementation limit I choose to have
	// Doesn't make things simpler, but it's lowkey necessary for shared mem
	if(len > 4096) {
		std::printf("[AHCI] Transfer too big\n");
		return false;
	} else if(((uint64_t)data) & 0xFFF) {
		std::printf("[AHCI] Buffer is not page aligned!\n");
		return false;
	}

	// Lock here! TODO

	// Get a command slot and prepare it
	auto slot = getSlot();
	PortStructs::CommandHeader* cmdHeader = &(region->cmdList.headers[slot]);
	*cmdHeader = PortStructs::CommandHeader(); // Reset
	cmdHeader->cfl = sizeof(FIS::H2D) / sizeof(uint32_t); // Dwords
	cmdHeader->atapi = 1;
	cmdHeader->write = 0; // This driver is read-only
	cmdHeader->prdtl = 1; // It's 2^12 max, and this becomes 2 at 2^22.
	cmdHeader->ctba = cmdTableOff;

	// Set FIS
	FIS::H2D* fis = (FIS::H2D*)&(region->cmdTable.cfis);
	*fis = FIS::H2D(); // Initialize again
	fis->c = 1; // Command
	fis->command = ATA_PACKET;
	fis->featureLo = 1; // DMA
	fis->control = 0x8; // ???
	fis->device = 0xA0; // Very legacy stuff
	//fis->lbaLo = len;

	// Copy the packet
	memcpy(region->cmdTable.acmd, cmd->packet, sizeof(ATAPI::CommandPacket));

	// Set the only PRD
	PortStructs::PRD* prd = &(region->cmdTable.prdt[0]);
	*prd = PortStructs::PRD();
	prd->dba = std::getPhys((uint64_t)data); // Where my data goes
	prd->dbc = len; // How much
	prd->irqOnCompletion = 0; // Shhh

	// Go!
	if(!execute(slot))
		return false;

	// All good
	return true;
}
