#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"

#define DEBUG_ENUMERATE_PHYSICAL_DEVICES

namespace ProjectA {
	class VulkanInstance final : public Instance, public ObjectWrapper<VkInstance> {
		public:
			explicit VulkanInstance(const ProjectDescriptor& projectDescriptor);

			~VulkanInstance() final override;

			void createDebugUtilsMessenger();

			VkPhysicalDevice selectPhysicalDevice() const;

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static void checkInstanceLayers();
			#endif

			uint32 calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) const;

			#ifdef DEBUG_ENABLE_MESSENGER
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		private:
			std::vector<const char8*> enabledExtensionNames;

			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static const std::vector<const char8*> enabledLayerNames;
			#endif

			#ifdef DEBUG_ENABLE_MESSENGER
				VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif
	};
}