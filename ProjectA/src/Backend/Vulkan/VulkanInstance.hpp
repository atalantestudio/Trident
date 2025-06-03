#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Instance.hpp"

namespace ProjectA {
	class VulkanInstance : public Instance, public ObjectWrapper<VkInstance> {
		public:
			VulkanInstance();

			~VulkanInstance() override;

		private:
			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static void checkInstanceLayers();
			#endif

			#ifdef BUILD_MODE_DEBUG
				void enumeratePhysicalDevices();
			#endif

			#ifdef DEBUG_ENABLE_MESSENGER
				static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
			#endif

		private:
			std::vector<const char*> enabledExtensionNames;

			#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
				static const std::vector<const char*> enabledLayerNames;
			#endif

			#ifdef DEBUG_ENABLE_MESSENGER
				VkDebugUtilsMessengerEXT debugUtilsMessenger;
			#endif
	};
}