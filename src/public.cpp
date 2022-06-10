#include <common.hpp>
#include <vector>
#include <unordered_map>
#include <shared_memory>

// This is all very static. Vector of DevicePorts, so each one has an index.
static std::vector<DevicePort> listedATAPIs;

size_t getATAPIs(std::PID client) {
	IGNORE(client);

	if(!listedATAPIs.size())
		for(auto const& x : atapis)
			listedATAPIs.push_back(x);

	return listedATAPIs.size();
}

static std::unordered_map<std::PID, char*> shared;

size_t connect(std::PID client, std::SMID smid) {
	// Already connected?
	if(!std::smRequest(client, smid))
		return false;

	char* ptr = (char*)std::smMap(smid);
	if(!ptr)
		return false;

	// TODO: unmap previous, release SMID
	shared[client] = ptr;
	return true;
}

size_t readATAPI(std::PID client, size_t id, size_t start, size_t sectors) {
	if(id >= listedATAPIs.size())
		return false;

	uint8_t* buffer = (uint8_t*)(shared[client]);
	return listedATAPIs[id].read(buffer, start, sectors);
}

void exportProcedures() {
	std::exportProcedure((void*)getATAPIs, 0);
	std::exportProcedure((void*)connect, 1);
	std::exportProcedure((void*)readATAPI, 3);
}
