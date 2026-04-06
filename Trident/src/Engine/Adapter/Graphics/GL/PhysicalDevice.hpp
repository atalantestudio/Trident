// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class GL::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const Trident::GraphicsAdapter* adapter);
	};

	namespace GL {
		ref<Trident::PhysicalDevice> selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu);

		view<char8> getPhysicalDeviceName(const Trident::PhysicalDevice* physicalDevice);
	}
}