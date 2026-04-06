// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/PhysicalDeviceLimits.hpp"

namespace Trident {
	class PhysicalDevice {
		public:
			/// Returns the name of the physical device.
			view<char8> getName() const;

			const PhysicalDeviceLimits& getLimits() const {
				return limits;
			}

			/// Creates and returns a logical device from this physical device.
			ref<Device> createDevice() const {
				return adapter->createDevice(this);
			}

		protected:
			explicit PhysicalDevice(const GraphicsAdapter* adapter) :
				adapter(adapter)
			{}

		protected:
			const GraphicsAdapter* adapter;

		public:
			PhysicalDeviceLimits limits;
	};
}