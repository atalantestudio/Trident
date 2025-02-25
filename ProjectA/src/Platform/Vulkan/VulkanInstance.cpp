#include "pch.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"

namespace ProjectA {
	// TODO: Pass enabled layer names as parameter.
	const std::vector<const char*> VulkanInstance::enabledLayerNames = {
		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			"VK_LAYER_KHRONOS_validation",
		#endif
	};

	// TODO: Automatically get enabled extension names.
	const std::vector<const char*> VulkanInstance::enabledExtensionNames = {
		#ifdef BUILD_MODE_DEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		#endif
	};

	VulkanInstance::VulkanInstance() : ObjectWrapper(nullptr) {
		PROFILE_SCOPE(FUNC_SIG);

		// https://registry.khronos.org/vulkan/specs/latest/man/html/VkApplicationInfo.html
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		// TODO: Application name. Same as window name?
		applicationInfo.pApplicationName = "ApplicationName";

		// TODO: Application version.
		applicationInfo.applicationVersion = VK_API_VERSION_1_0;

		// TODO: Engine name.
		applicationInfo.pEngineName = "EngineName";

		// TODO: Engine version.
		applicationInfo.engineVersion = VK_API_VERSION_1_0;

		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		#ifdef BUILD_MODE_DEBUG
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

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

		const VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &object);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan instance.");
		}

		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			// TODO: Check layers before creating instance?
			checkLayers();
		#endif
	}

	VulkanInstance::~VulkanInstance() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyInstance(object, nullptr);
	}

	#ifdef BUILD_MODE_DEBUG
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

	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		void VulkanInstance::checkLayers() {
			PROFILE_SCOPE(FUNC_SIG);

			uint32 layerPropertyCount;

			vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);

			std::vector<VkLayerProperties> layerProperties(layerPropertyCount);

			vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

			ENGINE_TRACE("Enabled layers:");

			for (const VkLayerProperties& layerProperties : layerProperties) {
				ENGINE_TRACE(layerProperties.layerName);
			}
		}
	#endif
}