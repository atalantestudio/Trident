#include "pch.hpp"

#include "Backend/Vulkan/VulkanInstance.hpp"

namespace ProjectA {
	// TODO: Client-side validation layer request.
	#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
		const std::vector<const char8*> VulkanInstance::enabledLayerNames = {
			"VK_LAYER_KHRONOS_validation",
		};
	#endif

	// NOTE: ~25Mb in Release mode.
	VulkanInstance::VulkanInstance(const ProjectDescriptor& projectDescriptor)
		#ifdef DEBUG_ENABLE_MESSENGER
			: debugUtilsMessengerCreateInfo{}
		#endif
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#ifdef DEBUG_ENABLE_VALIDATION_LAYERS
			checkInstanceLayers();
		#endif

		#ifdef DEBUG_ENABLE_MESSENGER
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
		applicationInfo.pApplicationName = &projectDescriptor.name[0];
		applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, projectDescriptor.version.major, projectDescriptor.version.minor, projectDescriptor.version.patch);
		applicationInfo.pEngineName = ENGINE_NAME;
		applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
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

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
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
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void VulkanInstance::createDebugUtilsMessenger() {
			const VkResult result = vkCreateDebugUtilsMessengerEXT(object, &debugUtilsMessengerCreateInfo, nullptr, &debugUtilsMessenger);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan debug messenger.");
		}
	#endif

	VulkanInstance::~VulkanInstance() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#ifdef DEBUG_ENABLE_MESSENGER
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

			/* for (uint32 layerPropertyIndex = 0; layerPropertyIndex < layerPropertyCount; layerPropertyIndex += 1) {
				ENGINE_INFO("- []", layerProperties[layerPropertyIndex].layerName);
			} */
		}
	#endif

	VkPhysicalDevice VulkanInstance::selectPhysicalDevice() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		uint32 physicalDeviceCount;
		VkResult result;

		result = vkEnumeratePhysicalDevices(object, &physicalDeviceCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan physical devices ([]).", string_VkResult(result));
		DEBUG_ASSERT(physicalDeviceCount > 0);

		sequence<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = vkEnumeratePhysicalDevices(object, &physicalDeviceCount, &physicalDevices[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices ([]).", string_VkResult(result));

		VkPhysicalDeviceProperties physicalDeviceProperties;
		uint32 maxPhysicalDeviceScore = 0;
		uint32 bestPhysicalDeviceIndex = 0;

		#ifdef DEBUG_ENUMERATE_PHYSICAL_DEVICES
			std::string physicalDeviceType;
		#endif

		#ifdef DEBUG_ENUMERATE_PHYSICAL_DEVICES
			ENGINE_INFO("Physical devices:");
		#endif

		for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.count(); physicalDeviceIndex += 1) {
			const VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

			vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

			const uint32 physicalDeviceScore = calculatePhysicalDeviceScore(physicalDeviceProperties);

			if (physicalDeviceScore >= maxPhysicalDeviceScore) {
				maxPhysicalDeviceScore = physicalDeviceScore;
				bestPhysicalDeviceIndex = physicalDeviceIndex;
			}

			#ifdef DEBUG_ENUMERATE_PHYSICAL_DEVICES
				switch (physicalDeviceProperties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						physicalDeviceType = "unknown type";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						physicalDeviceType = "integrated GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						physicalDeviceType = "discrete GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						physicalDeviceType = "virtual GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						physicalDeviceType = "CPU";

						break;
				}

				ENGINE_INFO("- [] ([])", physicalDeviceProperties.deviceName, physicalDeviceType);
			#endif
		}

		ENGINE_ASSERT(maxPhysicalDeviceScore > 0, "Could not select a suitable physical device.");

		return physicalDevices[bestPhysicalDeviceIndex];
	}

	uint32 VulkanInstance::calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) const {
		#if PREFERRED_GPU_TYPE == GPU_TYPE_DISCRETE
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				return 1;
			}
		#endif

		#if PREFERRED_GPU_TYPE == GPU_TYPE_INTEGRATED
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				return 1;
			}
		#endif

		return 0;
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		VkBool32 VulkanInstance::debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			switch (messageSeverity) {
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
					VULKAN_DEBUG(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
					VULKAN_INFO(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
					VULKAN_WARNING(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
					VULKAN_ERROR(pCallbackData->pMessage);

					break;
				}
			}

			return VK_FALSE;
		}
	#endif
}