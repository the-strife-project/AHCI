#ifndef FIS_HPP
#define FIS_HPP

// This file describes the structures for FIS, the communication method between
//   host and device, and viceversa.

namespace FIS {
	// FIS types
	#define FIS_TYPE_REG_H2D 0x27
	#define FIS_TYPE_REG_D2H 0x34

	// Host to device
	struct H2D {
		// First dword
		uint8_t type = FIS_TYPE_REG_H2D;
		uint8_t pmp : 4; // Port Multiplier Port
		uint8_t reserved0 : 3;
		uint8_t c : 1; // Command (1) or Control (0)
		uint8_t command = 0; // Command register
		uint8_t featureLo = 0; // Feature register (LSB)

		// Second dword
		uint32_t lbaLo : 24; // Little-endian
		uint8_t device = 0; // Device register

		// Third dword
		uint32_t lbaHi : 24; // Little-endian too
		uint8_t featureHi = 0; // Feature register (MSB)

		// Fourt dword
		uint16_t count = 0; // Count register
		uint8_t icc = 0; // Isochronous command completion
		uint8_t control = 0; // Control register

		uint32_t reserved1 = 0; // Fifth dword

		H2D() {
			pmp = reserved0 = c = 0;
			lbaLo = lbaHi = 0;
		}
	} __attribute__((packed));
}

#endif
