// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ENUMERATE_PHYSICAL_DEVICES
#endif

namespace ProjectA {
	class VK::Instance final : public ProjectA::Instance {
		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static void checkInstanceLayers();
			#endif

			#ifdef DEBUG_ENABLE_MESSENGER
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		public:
			explicit Instance(const ProjectDescriptor& projectDescriptor, const ApplicationProperties& properties);

			~Instance() final override;

			#ifdef DEBUG_ENABLE_MESSENGER
				void createDebugUtilsMessenger();
			#endif

			VkPhysicalDevice selectPhysicalDevice() const;

		private:
			uint32 calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) const;

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static const sequence<const char8*> ENABLED_LAYER_NAMES;
			#endif

		public:
			VkInstance instance;

		private:
			#ifdef DEBUG_ENABLE_MESSENGER
				VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif

			std::vector<const char8*> enabledExtensionNames;
			bool preferIntegratedGpu;
	};
}