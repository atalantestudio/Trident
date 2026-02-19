// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/PhysicalDeviceLimits.hpp"

namespace Trident {
	class PhysicalDevice {
		public:
			virtual ~PhysicalDevice() = default;

			ref<Device> createDevice() const {
				return adapter->createDevice(this);
			}

			const PhysicalDeviceLimits& getLimits() const {
				return limits;
			}

		protected:
			explicit PhysicalDevice(const GraphicsAdapter* adapter) :
				adapter(adapter)
			{}

		protected:
			PhysicalDeviceLimits limits;

		private:
			const GraphicsAdapter* adapter;
	};
}