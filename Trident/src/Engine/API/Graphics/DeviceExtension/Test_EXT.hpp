// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"

namespace Trident {
	// TODO(test/graphics/device/extension)
	template<typename Device>
	class DeviceExtension<Test_EXT, Device> {
		public:
			void test_EXT() {
				Device* device = static_cast<Device*>(this);

				device->adapter->test_EXT_fooImplementation();
			}
	};
}