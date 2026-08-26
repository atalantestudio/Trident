// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"

namespace Trident {
	template<typename Device>
	class DeviceExtension<EXT_hardware_ray_tracing, Device> {
		public:
			/* void test_EXT() {
				Device* device = static_cast<Device*>(this);

				device->adapter->test_EXT_fooImplementation();
			} */
	};
}