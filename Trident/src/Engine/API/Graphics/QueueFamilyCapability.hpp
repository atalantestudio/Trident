// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// Capabilities of queues in a queue family.
	// Used when querying the corresponding queue from a device.
	// In general, graphics queues accept any command, compute queues accept compute and transfer related commands and transfer queues accept only transfer commands.
	enum class QueueFamilyCapability : uint8 {
		GRAPHICS,
		COMPUTE,
		TRANSFER,

		count,
	};
}