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

			/// Returns a report of the physical device's limits.
			const PhysicalDeviceLimits& getLimits() const;

			/// Creates and returns a logical device from this physical device.
			ref<Device> createDevice() const;

		protected:
			explicit PhysicalDevice(const GraphicsAdapter* adapter);

		protected:
			const GraphicsAdapter* adapter;

		public:
			PhysicalDeviceLimits limits;
	};
}