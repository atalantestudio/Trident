// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"

#if defined(BUILD_MODE_DEBUG)
	#define DEBUG_ENUMERATE_PHYSICAL_DEVICES
#endif

namespace Trident {
	class VK::Instance final : public Trident::Instance {
		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter);

			void loadProcedures();

		public:
			VkInstance instance;

			#if defined(DEBUG_ENABLE_MESSENGER)
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif

			PFN_vkCreateDevice vkCreateDevice;

			#if defined(OPERATING_SYSTEM_WINDOWS)
				PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
			#endif

			PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
			PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
			PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
			PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
			PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
			PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
			PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
			PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
			PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
			PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
			PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
	};

	namespace VK {
		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		void destroyInstance(const Trident::Instance* instance);
	}
}