// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/Instance.hpp"

namespace Trident {
	GL::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter) :
		Trident::PhysicalDevice(adapter)
	{}

	ref<Trident::PhysicalDevice> GL::selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if defined(BUILD_MODE_DEBUG)
			if (preferIntegratedGpu) {
				instance->adapter->logger.warning("GPU selection can only be done at compile-time. Please enable the GL_PREFER_INTEGRATED_GPU macro instead.");
			}
		#endif

		ref<GL::PhysicalDevice> physicalDevice = createRef<GL::PhysicalDevice>(instance->adapter);

		return physicalDevice;
	}
}