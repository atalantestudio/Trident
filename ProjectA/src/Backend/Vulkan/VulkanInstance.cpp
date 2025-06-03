#include "pch.hpp"

#include "Backend/Vulkan/VulkanInstance.hpp"

namespace ProjectA {
	// TODO: Client-side validation layer request.
	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		const std::vector<const char*> VulkanInstance::enabledLayerNames = {
			"VK_LAYER_KHRONOS_validation",
		};
	#endif

	// This constructor seems to allocate ~25Mb of memory in Release mode.
	VulkanInstance::VulkanInstance() {
		PROFILE_SCOPE(FUNC_SIG);

		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			// TODO: Check layers before creating instance?
			checkInstanceLayers();
		#endif

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

		applicationInfo.pEngineName = ENGINE_NAME;

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

		#ifdef PLATFORM_WINDOWS
			enabledExtensionNames.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
			enabledExtensionNames.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#endif

		#ifdef DEBUG_ENABLE_MESSENGER
			enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

		VkResult result;

		result = vkCreateInstance(&instanceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance ([]).", string_VkResult(result));

		#ifdef DEBUG_ENABLE_MESSENGER
			const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(object, "vkCreateDebugUtilsMessengerEXT"));

			ENGINE_ASSERT(vkCreateDebugUtilsMessengerEXT != nullptr, "Failed to get the address of PFN_vkCreateDebugUtilsMessengerEXT.");

			result = vkCreateDebugUtilsMessengerEXT(object, &debugUtilsMessengerCreateInfo, nullptr, &debugUtilsMessenger);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan debug messenger.");
		#endif

		#ifdef BUILD_MODE_DEBUG
			enumeratePhysicalDevices();
		#endif
	}

	VulkanInstance::~VulkanInstance() {
		PROFILE_SCOPE(FUNC_SIG);

		#ifdef DEBUG_ENABLE_MESSENGER
			const PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(object, "vkDestroyDebugUtilsMessengerEXT"));

			ENGINE_ASSERT(vkDestroyDebugUtilsMessengerEXT != nullptr, "Failed to get the address of PFN_vkDestroyDebugUtilsMessengerEXT.");

			vkDestroyDebugUtilsMessengerEXT(object, debugUtilsMessenger, nullptr);
		#endif

		vkDestroyInstance(object, nullptr);
	}

	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		void VulkanInstance::checkInstanceLayers() {
			uint32 layerPropertyCount;
			VkResult result;

			result = vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan instance layer properties ([]).", string_VkResult(result));

			std::vector<VkLayerProperties> layerProperties(layerPropertyCount);

			result = vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan instance layer properties ([]).", string_VkResult(result));

			/*for (uint32 i = 0; i < layerPropertyCount; i += 1) {
				ENGINE_INFO("- []", layerProperties[i].layerName);
			}*/
		}
	#endif

	#ifdef BUILD_MODE_DEBUG
		void VulkanInstance::enumeratePhysicalDevices() {
			uint32 physicalDeviceCount;
			VkResult result;

			result = vkEnumeratePhysicalDevices(object, &physicalDeviceCount, nullptr);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan physical devices ([]).", string_VkResult(result));

			DEBUG_ASSERT(physicalDeviceCount > 0);

			fchain<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

			result = vkEnumeratePhysicalDevices(object, &physicalDeviceCount, &physicalDevices[0]);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices ([]).", string_VkResult(result));

			VkPhysicalDeviceProperties physicalDeviceProperties;

			ENGINE_TRACE("Available physical devices:");

			for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.count; physicalDeviceIndex += 1) {
				const VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

				vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

				ENGINE_DEBUG("[]. []", physicalDeviceIndex, physicalDeviceProperties.deviceName);
			}
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

			// TODO: Check https://vulkan.lunarg.com/doc/sdk/1.4.309.0/windows/error_messages.html to improve logging.
			return VK_FALSE;
		}
	#endif
}