// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/ExtendedDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/PhysicalDeviceLimits.hpp"

namespace Trident {
	class PhysicalDevice {
		public:
			/// Returns the name of the physical device.
			view<char8> getName() const;

			/// Returns the number of supported memory types.
			uint32 getMemoryTypeCount() const;

			/// Returns a report of the physical device's limits.
			const PhysicalDeviceLimits& getLimits() const;

			/// Creates and returns a logical device linked to this physical device.
			/// The resulting device exposes core and extension methods to the user.
			template<DeviceExtensionName... Extensions>
			inline ref<ExtendedDevice<Extensions...>> createDevice() const {
				static constexpr DeviceExtensionName extensions[] = {Extensions..., static_cast<DeviceExtensionName>(0)};
				static constexpr view<DeviceExtensionName> extensionView(extensions, sizeof...(Extensions));

				ref<Device> device = adapter->createDeviceImplementation(adapter, this, extensionView);
				device->memoryAllocator = adapter->createMemoryAllocatorImplementation(device);

				return ref_owning_cast<ExtendedDevice<Extensions...>>(device);
			}

		protected:
			explicit PhysicalDevice(const GraphicsAdapter* adapter);

		protected:
			const GraphicsAdapter* adapter;

		public:
			PhysicalDeviceLimits limits;
	};
}