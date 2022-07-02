#include <common.hpp>
#include <vector>
#include <unordered_map>
#include <shared_memory>
#include <mutex>

// This is all very static. Vector of DevicePorts, so each one has an index.
static std::vector<DevicePort> listedATAPIs;
static std::mutex listedATAPIsLock;

size_t getATAPIs(std::PID client) {
	IGNORE(client);

	listedATAPIsLock.acquire();
	if(!listedATAPIs.size())
		for(auto const& x : atapis)
			listedATAPIs.push_back(x);

	size_t ret = listedATAPIs.size();
	listedATAPIsLock.release();
	return ret;
}

static std::unordered_map<std::PID, char*> shared;
static std::mutex sharedLock;

size_t connect(std::PID client, std::SMID smid) {
	// Already connected?
	if(!std::smRequest(client, smid))
		return false;

	char* ptr = (char*)std::smMap(smid);
	if(!ptr)
		return false;

	// TODO: unmap previous, release SMID
	sharedLock.acquire();
	shared[client] = ptr;
	sharedLock.release();
	return true;
}

size_t readATAPI(std::PID client, size_t id, size_t start, size_t sectors) {
	listedATAPIsLock.acquire();
	if(id >= listedATAPIs.size() ) {
		listedATAPIsLock.release();
		return false;
	}

	DevicePort& dp = listedATAPIs[id];
	listedATAPIsLock.release();

	sharedLock.acquire();
	if(shared.find(client) == shared.end()) {
		sharedLock.release();
		return 0;
	}

	uint8_t* buffer = (uint8_t*)(shared[client]);
	sharedLock.release();

	return dp.read(buffer, start, sectors);
}

void exportProcedures() {
	std::exportProcedure((void*)getATAPIs, 0);
	std::exportProcedure((void*)connect, 1);
	std::exportProcedure((void*)readATAPI, 3);
}
