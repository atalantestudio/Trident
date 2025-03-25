#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Instance.hpp"

namespace ProjectA {
	class VulkanInstance : public Instance, public ObjectWrapper<VkInstance> {
		public:
			VulkanInstance();

			~VulkanInstance() override;

		public:
			chain<const char*> enabledExtensionNames;

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static void checkInstanceLayers();
			#endif

			#ifdef DEBUG_ENABLE_MESSENGER
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static const chain<const char*> enabledLayerNames;
			#endif

			VkDebugUtilsMessengerEXT debugUtilsMessenger;
	};
}