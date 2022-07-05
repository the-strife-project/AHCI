#ifndef ABSTRACTIONS_HPP
#define ABSTRACTIONS_HPP

#include <Port/Port.hpp>
#include <functional>

class DevicePort {
private:
	uint32_t device; // HBA PCI address
	uint8_t portid; // Port inside of the HBA

	Port port; // Pointer to actual port structure

public:
	DevicePort() = default;
	DevicePort(uint32_t device, uint8_t portid, const Port& port)
		: device(device), portid(portid), port(port)
	{}

	inline std::Hash hash() const { return (device << 5) | portid; }
	inline bool operator==(const DevicePort& other) const {
		return (device == other.device) && (portid == other.portid);
	}

	// Actually useful methods
	void init() { port.init(); }

	bool read(uint8_t* data, size_t lba, size_t sectors);

	inline bool operator<(const DevicePort& other) const {
		return hash() < other.hash();
	}
};

namespace std {
	template<> struct hash<DevicePort> {
		inline Hash operator()(const DevicePort& x) const noexcept {
			return x.hash();
		}
	};
}

#endif
