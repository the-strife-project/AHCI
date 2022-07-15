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

size_t readATAPI(std::PID client, std::SMID smid, size_t id, size_t lba) {
	listedATAPIsLock.acquire();
	if(id >= listedATAPIs.size()) {
		listedATAPIsLock.release();
		return false;
	}

	DevicePort& dp = listedATAPIs[id];
	listedATAPIsLock.release();

	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return 0;

	size_t sectors = npages * PAGE_SIZE / dp.getSectorSize();

	size_t ret = dp.read(link.f, lba, sectors);
	std::sm::unlink(smid);
	return ret;
}

void exportProcedures() {
	std::exportProcedure((void*)getATAPIs, 0);
	std::exportProcedure((void*)readATAPI, 3);
}
