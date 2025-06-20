#pragma once

namespace ProjectA {
	// Capabilities of queues in a queue family.
	// Used when querying the corresponding queue from a device.
	enum class QueueFamilyCapability : uint8 {
		GRAPHICS = 0,
		COMPUTE = 1,
		TRANSFER = 2,
	};
}