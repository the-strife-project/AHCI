#ifndef PORT_HPP
#define PORT_HPP

#include <HBA/HBA.hpp>
#include "structures.hpp"
#include "ATAPI/ATAPI.hpp"
#include "FIS.hpp"

// Communication with an AHCI port
class Port {
private:
	HBA::Port* port;

	// Regarding the physical memory region
	uint64_t phys;
	PortStructs::PhysRegion* region;
	uint64_t cmdTableOff; // Physical

	inline FIS::H2D* getH2D() {
		auto* ret = (FIS::H2D*)&(region->cmdTable.cfis);
		*ret = FIS::H2D();
		return ret;
	}

public:
	Port() = default;
	Port(HBA::Port* port) : port(port) {}

	// Stop and start command engine
	void stop();
	bool start();

	// For sending commands (low level)
	uint8_t getSlot();
	void waitReady();

	// For sending commands (high level)
	bool execute(uint8_t slot);

	// Actual commands
	bool sendATAPI(ATAPI::CommandPacket*, uint8_t*, size_t);

	// Higher level
	bool init();
};

#endif
