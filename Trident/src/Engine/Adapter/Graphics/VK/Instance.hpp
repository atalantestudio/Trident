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
		private:
			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				static void checkInstanceLayers();
			#endif

			#if defined(DEBUG_ENABLE_MESSENGER)
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		public:
			explicit Instance(const InstanceDescriptor& descriptor);

			~Instance() final override;

			#if defined(DEBUG_ENABLE_MESSENGER)
				void createDebugUtilsMessenger();
			#endif

			VkPhysicalDevice selectPhysicalDevice() const;

		private:
			uint32 calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) const;

		private:
			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				static const sequence<const char8*> ENABLED_LAYER_NAMES;
			#endif

		public:
			VkInstance instance;

		private:
			#if defined(DEBUG_ENABLE_MESSENGER)
				VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif

			std::vector<const char8*> enabledExtensionNames;
			bool preferIntegratedGpu;
	};
}