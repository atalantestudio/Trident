#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Backend/Vulkan/VulkanInstance.hpp"

namespace ProjectA {
	// TODO: Client-side validation layer request.
	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		const std::vector<const char*> VulkanInstance::enabledLayerNames = {
			"VK_LAYER_KHRONOS_validation",
		};
	#endif

	VulkanInstance::VulkanInstance() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanInstance]");

		#ifdef DEBUG_ENABLE_MESSENGER
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
			debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			// TODO: Allow changing message severity?
			debugUtilsMessengerCreateInfo.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			// TODO: Allow changing message type?
			debugUtilsMessengerCreateInfo.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

			debugUtilsMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerUserCallback;
			debugUtilsMessengerCreateInfo.pUserData = nullptr;
		#endif

		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		// TODO: Automatically determine application name. Same as window name?
		applicationInfo.pApplicationName = "ApplicationName";

		// TODO: Automatically determine application version.
		applicationInfo.applicationVersion = VK_API_VERSION_1_0;

		// TODO: Automatically determine engine name.
		applicationInfo.pEngineName = "EngineName";

		// TODO: Automatically determine engine version.
		applicationInfo.engineVersion = VK_API_VERSION_1_0;

		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		#ifdef DEBUG_ENABLE_MESSENGER
			instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
		#else
			instanceCreateInfo.pNext = nullptr;
		#endif

		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			instanceCreateInfo.enabledLayerCount = static_cast<uint32>(enabledLayerNames.size());
			instanceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();
		#else
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.ppEnabledLayerNames = nullptr;
		#endif

		uint32 glfwRequiredExtensionCount;

		const char** glfwRequiredExtensionNames = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

		enabledExtensionNames.assign(glfwRequiredExtensionNames, glfwRequiredExtensionNames + glfwRequiredExtensionCount);

		#ifdef DEBUG_ENABLE_MESSENGER
			enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan instance.");
		}

		#ifdef DEBUG_ENABLE_MESSENGER
			const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(object, "vkCreateDebugUtilsMessengerEXT"));

			if (vkCreateDebugUtilsMessengerEXT == nullptr) {
				throw std::runtime_error("Vulkan debug messenger extension not present.");
			}

			if (vkCreateDebugUtilsMessengerEXT(object, &debugUtilsMessengerCreateInfo, nullptr, &debugUtilsMessenger) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan debug messenger.");
			}
		#endif

		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			// TODO: Check layers before creating instance?
			checkInstanceLayers();
		#endif
	}

	VulkanInstance::~VulkanInstance() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanInstance]");

		#ifdef DEBUG_ENABLE_MESSENGER
			const PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(object, "vkDestroyDebugUtilsMessengerEXT"));

			if (vkDestroyDebugUtilsMessengerEXT == nullptr) {
				return;
			}

			vkDestroyDebugUtilsMessengerEXT(object, debugUtilsMessenger, nullptr);
		#endif

		vkDestroyInstance(object, nullptr);
	}

	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		void VulkanInstance::checkInstanceLayers() {
			uint32 layerPropertyCount;

			vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);

			chain<VkLayerProperties> layerProperties(layerPropertyCount);

			vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

			/*ENGINE_TRACE("Instance layers:");

			for (uint32 i = 0; i < layerPropertyCount; i++) {
				ENGINE_TRACE("- []", layerProperties[i].layerName);
			}*/
		}
	#endif

	#ifdef DEBUG_ENABLE_MESSENGER
		VkBool32 VulkanInstance::debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			switch (messageSeverity) {
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
					ENGINE_TRACE(pCallbackData->pMessage);

					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					ENGINE_INFO(pCallbackData->pMessage);

					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					ENGINE_WARN(pCallbackData->pMessage);

					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					ENGINE_ERROR(pCallbackData->pMessage);

					break;
			}

			return VK_FALSE;
		}
	#endif
}