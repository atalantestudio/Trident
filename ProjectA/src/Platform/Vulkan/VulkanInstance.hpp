#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Instance.hpp"

namespace ProjectA {
	class VulkanInstance : public Instance, public ObjectWrapper<VkInstance> {
		public:
			VulkanInstance();

			~VulkanInstance() override;
		private:
			#ifdef BUILD_MODE_DEBUG
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static void checkLayers();
			#endif

		private:
			static const std::vector<const char*> enabledLayerNames;
			static const std::vector<const char*> enabledExtensionNames;
	};
}