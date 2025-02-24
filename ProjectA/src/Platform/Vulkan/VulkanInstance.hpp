#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"

namespace ProjectA {
	class VulkanInstance : public ObjectWrapper<VkInstance> {
		public:
			VulkanInstance();

			~VulkanInstance();
		private:
			#ifdef BUILD_MODE_DEBUG
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				void checkLayers() const;
			#endif

		private:
			static const std::vector<const char*> enabledLayerNames;
			static const std::vector<const char*> enabledExtensionNames;
	};
}