// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Instance.hpp"

namespace Trident {
	GL::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter) :
		Trident::PhysicalDevice(adapter)
	{}

	ref<Trident::PhysicalDevice> GL::selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if defined(BUILD_MODE_DEBUG)
			if (preferIntegratedGpu) {
				ADAPTER_WARNING("GPU selection can only be done at compile-time. Please define GL_PREFER_INTEGRATED_GPU instead.");
			}
		#endif

		ref<GL::PhysicalDevice> physicalDevice = createRef<GL::PhysicalDevice>(instance->adapter);

		return physicalDevice;
	}

	uint32 GL::getPhysicalDeviceMemoryTypeCountImplementation(const Trident::PhysicalDevice* physicalDevice) {
		return 1;
	}

	view<char8> GL::getPhysicalDeviceName(const Trident::PhysicalDevice* physicalDevice) {
		// TODO(graphics/gl/load-procedures): Find a way around this issue.
		ADAPTER_ASSERT(glGetString != nullptr, "Procedures can only be loaded after a surface is created.");

		return pointer_cast<const GLchar*>(glGetString(GL_RENDERER));
	}
}