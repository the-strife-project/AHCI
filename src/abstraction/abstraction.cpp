#include "abstraction.hpp"

bool DevicePort::read(uint8_t* data, size_t lba, size_t sectors) {
	union {
		ATAPI::Read read;
		ATAPI::CommandPacket packet;
	} u;

	u.read = ATAPI::Read(lba, sectors);
	bool result = port.sendATAPI(&(u.packet), data, ATAPI_SECTOR_SIZE*sectors);
	if(result)
		return true;

	// Try again
	return port.sendATAPI(&(u.packet), data, ATAPI_SECTOR_SIZE*sectors);
}
