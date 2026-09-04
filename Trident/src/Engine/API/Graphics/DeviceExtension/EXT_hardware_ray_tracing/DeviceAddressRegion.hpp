// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct DeviceAddressRegion {
		uint64 deviceAddress;
		uint64 stride;
		uint64 size;
	};
}