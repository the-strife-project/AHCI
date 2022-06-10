#ifndef COMMON_HPP
#define COMMON_HPP

#include <unordered_set>
#include <cstdio>

// AHCI PCI device identification
#define AHCI_PCI_CLASS 0x01
#define AHCI_PCI_SUBCLASS 0x06

// PCI's BAR 5 is ABAR
#define AHCI_PCI_ABAR_BAR 5

#include <HBA/HBA.hpp>
#include "abstraction/abstraction.hpp"
#include <Port/Port.hpp>

extern std::unordered_set<DevicePort> atapis;

void probe();

void exportProcedures();

#endif
