// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtensionName.hpp"

namespace Trident::VK {
	inline void enableDeviceExtension(DeviceExtensionName extension, std::vector<const char8*>& enabledExtensionNames) {
		switch (extension) {
			case EXT_hardware_ray_tracing: {
				enabledExtensionNames.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
				enabledExtensionNames.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
				enabledExtensionNames.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

				break;
			}
		}
	}
}