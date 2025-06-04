#include "pch.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Backend/Win32/Surface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(VkInstance instance, const Window& window) :
		instance(instance),
		window(window)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result;

		#ifdef PLATFORM_WINDOWS
			result = createVulkanWin32Surface(window, instance, nullptr, &object);
		#else
			#error "Unhandled platform."
		#endif

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface ([]).", string_VkResult(result));
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroySurfaceKHR(instance, object, nullptr);
	}

	void VulkanSurface::initPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, uint32 graphicsQueueFamilyIndex) {
		VkBool32 supported;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyIndex, object, &supported);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan physical device surface support ([]).", string_VkResult(result));

		if (supported) {
			presentQueueFamilyIndex = graphicsQueueFamilyIndex;
		}
	}

	VkSurfaceFormatKHR VulkanSurface::selectFormat(VkPhysicalDevice physicalDevice) const {
		uint32 formatCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface format count ([]).", string_VkResult(result));

		fchain<VkSurfaceFormatKHR> formats(formatCount);

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, &formats[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface formats ([]).", string_VkResult(result));

		VkSurfaceFormatKHR selectedFormat{};
		selectedFormat.format = VK_FORMAT_UNDEFINED;
		selectedFormat.colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

		//const VkFormat desiredFormat = VK_FORMAT_B8G8R8A8_SRGB;
		const VkFormat desiredFormat = VK_FORMAT_B8G8R8A8_UNORM;

		for (const VkSurfaceFormatKHR format : formats) {
			if (format.format == desiredFormat) {
				selectedFormat = format;

				break;
			}
		}

		ENGINE_ASSERT(selectedFormat.format != VK_FORMAT_UNDEFINED, "Could not select a Vulkan surface frormat.");

		return selectedFormat;
	}

	VkPresentModeKHR VulkanSurface::selectPresentMode(VkPhysicalDevice physicalDevice) const {
		uint32 presentModeCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present mode count ([]).", string_VkResult(result));

		fchain<VkPresentModeKHR> presentModes(presentModeCount);

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, &presentModes[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present modes ([]).", string_VkResult(result));

		// VK_PRESENT_MODE_FIFO_KHR is guaranteed to be always available.
		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const VkPresentModeKHR presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				selectedPresentMode = presentMode;

				break;
			}
		}

		return selectedPresentMode;
	}

	VkSurfaceCapabilitiesKHR VulkanSurface::getCapabilities(VkPhysicalDevice physicalDevice) const {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, object, &surfaceCapabilities);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface capabilities ([]).", string_VkResult(result));

		return surfaceCapabilities;
	}

	ImageFormat VulkanSurface::getFormat() const {
		// TODO
		return ImageFormat::B8G8R8A8_UNORM;
	}
}