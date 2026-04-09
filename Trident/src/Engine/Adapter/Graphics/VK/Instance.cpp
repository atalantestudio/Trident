// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	// TODO: Expose to the client side.
	static const view<const char8*> ENABLED_LAYER_NAMES = {
		#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			"VK_LAYER_KHRONOS_validation",
		#endif
	};

	#if defined(DEBUG_ENABLE_MESSENGER)
		static VkBool32 debugUtilsMessengerUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			switch (messageSeverity) {
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
					ADAPTER_TRACE(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
					ADAPTER_INFO(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
					ADAPTER_WARNING(pCallbackData->pMessage);

					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
					ADAPTER_ERROR(pCallbackData->pMessage);

					break;
				}
			}

			return VK_FALSE;
		}
	#endif

	VK::Instance::Instance(const Trident::GraphicsAdapter* adapter) :
		Trident::Instance(adapter)
	{}

	void VK::Instance::loadProcedures() {
		#define LOAD_INSTANCE_PROCEDURE(procedure) procedure = loadInstanceProcedure<decltype(procedure)>(instance, #procedure)

		LOAD_INSTANCE_PROCEDURE(vkCreateDevice);

		#if defined(OPERATING_SYSTEM_WINDOWS)
			LOAD_INSTANCE_PROCEDURE(vkCreateWin32SurfaceKHR);
		#endif

		LOAD_INSTANCE_PROCEDURE(vkDestroySurfaceKHR);
		LOAD_INSTANCE_PROCEDURE(vkEnumerateDeviceExtensionProperties);
		LOAD_INSTANCE_PROCEDURE(vkEnumeratePhysicalDevices);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceFeatures);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceMemoryProperties);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceProperties);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceQueueFamilyProperties);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceSurfaceFormatsKHR);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceSurfacePresentModesKHR);
		LOAD_INSTANCE_PROCEDURE(vkGetPhysicalDeviceSurfaceSupportKHR);

		#undef LOAD_INSTANCE_PROCEDURE
	}

	ref<Trident::Instance> VK::createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<VK::Instance> instance = createRef<VK::Instance>(adapter);
		VkResult result;

		// Load instance procedures.
		#define LOAD_PROCEDURE(instance, procedure) loadInstanceProcedure<decltype(procedure)>(instance, #procedure)

		static PFN_vkCreateInstance vkCreateInstance = LOAD_PROCEDURE(nullptr, vkCreateInstance);
		static PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = LOAD_PROCEDURE(nullptr, vkEnumerateInstanceLayerProperties);

		#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			{
				uint32 layerPropertyCount;

				result = vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to find the number of instance layer properties ([]).", string_VkResult(result));

				std::vector<VkLayerProperties> layerProperties(layerPropertyCount);

				result = vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to enumerate instance layer properties ([]).", string_VkResult(result));

				for (const VkLayerProperties& layerProperties : layerProperties) {
					ADAPTER_INFO("- []", layerProperties.layerName);
				}
			}
		#endif

		#if defined(DEBUG_ENABLE_MESSENGER)
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
			debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCreateInfo.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCreateInfo.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerUserCallback;
			debugUtilsMessengerCreateInfo.pUserData = nullptr;
		#endif

		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = &descriptor.projectName[0];
		applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, descriptor.projectVersion.major, descriptor.projectVersion.minor, descriptor.projectVersion.patch);
		applicationInfo.pEngineName = ENGINE_NAME;
		applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		#if defined(DEBUG_ENABLE_MESSENGER)
			instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
		#else
			instanceCreateInfo.pNext = nullptr;
		#endif

		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			instanceCreateInfo.enabledLayerCount = static_cast<uint32>(ENABLED_LAYER_NAMES.count());
			instanceCreateInfo.ppEnabledLayerNames = &ENABLED_LAYER_NAMES[0];
		#else
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.ppEnabledLayerNames = nullptr;
		#endif

		std::vector<const char8*> enabledExtensionNames;

		#if defined(DEBUG_ENABLE_MESSENGER)
			enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		enabledExtensionNames.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			enabledExtensionNames.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#endif

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

		result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance->instance);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create instance ([]).", string_VkResult(result));

		#if defined(DEBUG_ENABLE_MESSENGER)
			const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = LOAD_PROCEDURE(instance->instance, vkCreateDebugUtilsMessengerEXT);

			result = vkCreateDebugUtilsMessengerEXT(instance->instance, &debugUtilsMessengerCreateInfo, nullptr, &instance->debugUtilsMessenger);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create debug messenger ([]).", string_VkResult(result));
		#endif

		#undef LOAD_PROCEDURE

		instance->loadProcedures();

		return instance;
	}

	void VK::destroyInstance(const Trident::Instance* instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkDestroyInstance vkDestroyInstance = loadInstanceProcedure<PFN_vkDestroyInstance>(static_cast<const VK::Instance*>(instance)->instance, "vkDestroyInstance");

		const VK::Instance* _instance = static_cast<const VK::Instance*>(instance);

		#if defined(DEBUG_ENABLE_MESSENGER)
			static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = loadInstanceProcedure<PFN_vkDestroyDebugUtilsMessengerEXT>(static_cast<const VK::Instance*>(instance)->instance, "vkDestroyDebugUtilsMessengerEXT");

			vkDestroyDebugUtilsMessengerEXT(_instance->instance, _instance->debugUtilsMessenger, nullptr);
		#endif

		vkDestroyInstance(_instance->instance, nullptr);
	}
}