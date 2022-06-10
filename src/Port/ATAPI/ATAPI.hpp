#ifndef ATAPI_HPP
#define ATAPI_HPP

// The two most important ATA command sent to ATAPI devices
#define ATA_PACKET 0xA0
//#define ATA_IDENTIFY 0xA1
#define ATA_IDENTIFY_PACKET_DEVICE 0xA1

// SCSI command set, for PACKET ATA commands
#define SCSI_READTOC 0x43
#define SCSI_READ12 0xA8

#define ATAPI_SECTOR_SIZE 2048

namespace ATAPI {
	struct Read {
		uint8_t scsiCommand;
		uint8_t dontKnow0;
		uint8_t lba[4]; // Big-endian
		uint8_t sectors[4]; // Big-endian
		uint8_t dontKnow1[2];

		Read() = default;
		Read(uint32_t lba_, uint8_t sectors_)
			: scsiCommand(SCSI_READ12), dontKnow0(0),
			  dontKnow1{0, 0}
		{
			lba[0] = (lba_ >> (3*8)) & 0xFF;
			lba[1] = (lba_ >> (2*8)) & 0xFF;
			lba[2] = (lba_ >> (1*8)) & 0xFF;
			lba[3] = (lba_ >> (0*8)) & 0xFF;

			sectors[0] = sectors[1] = sectors[2] = 0;
			sectors[3] = sectors_;
		}
	} __attribute__((packed));

	struct CommandPacket {
		uint8_t packet[16];
	} __attribute__((packed));
};

#endif
