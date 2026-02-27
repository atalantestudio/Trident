// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"

#if defined(BUILD_MODE_DEBUG)
	#define DEBUG_ENUMERATE_PHYSICAL_DEVICES
#endif

namespace Trident {
	class VK::Instance final : public Trident::Instance {
		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter);

		public:
			VkInstance instance;

			#if defined(DEBUG_ENABLE_MESSENGER)
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif

			PFN_vkDestroyInstance vkDestroyInstance;
			PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;

			#if defined(OPERATING_SYSTEM_WINDOWS)
				PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
			#endif

			#if defined(DEBUG_ENABLE_MESSENGER)
				PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
				PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
			#endif
	};

	namespace VK {
		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		void destroyInstance(const Trident::Instance* instance);
	}
}