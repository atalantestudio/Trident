#pragma once

namespace ProjectA {
	struct PhysicalDeviceLimits {
		// TODO: Use vec3<sint32>.
		// Maximum number of local workgroups that can be dispatched by a single dispatching command.
		uint32 maxComputeWorkgroupCount[3];
	};
}