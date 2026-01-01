#include "pch.hpp"

#include "Backend/VK/ImageFormat.hpp"
#include "Backend/VK/Surface.hpp"
#include "Backend/Win32/Surface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(VkInstance instance, const Window& window) :
		instance(instance),
		window(window)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkResult result;

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			result = createVulkanWin32Surface(window, instance, nullptr, &object);
		#else
			#error "Unhandled platform."
		#endif

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface ([]).", string_VkResult(result));
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroySurfaceKHR(instance, object, nullptr);
	}

	void VulkanSurface::setPhysicalDevice(VkPhysicalDevice physicalDevice) {
		this->physicalDevice = physicalDevice;
	}

	void VulkanSurface::checkPresentQueueFamilyIndex(uint32 graphicsQueueFamilyIndex) {
		VkBool32 supported;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyIndex, object, &supported);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan physical device surface support ([]).", string_VkResult(result));
		ENGINE_ASSERT(supported == VK_TRUE, "The graphics queue family does not support this surface.");
	}

	ImageFormat VulkanSurface::getFormat() const {
		const VkSurfaceFormatKHR surfaceFormat = selectFormat();

		return fromVkFormat(surfaceFormat.format);
	}

	VkSurfaceFormatKHR VulkanSurface::selectFormat() const {
		uint32 formatCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface format count ([]).", string_VkResult(result));

		sequence<VkSurfaceFormatKHR> formats(formatCount);

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

	VkPresentModeKHR VulkanSurface::selectPresentMode(bool vSync) const {
		uint32 presentModeCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present mode count ([]).", string_VkResult(result));

		sequence<VkPresentModeKHR> presentModes(presentModeCount);

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, &presentModes[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present modes ([]).", string_VkResult(result));

		if (!vSync) {
			// Try to find IMMEDIATE.
			for (const VkPresentModeKHR presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
					return presentMode;
				}
			}

			ENGINE_WARNING("V-Sync is disabled but no matching present mode was found. Reverting to FIFO present mode.");

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		// Try to find MAILBOX, else revert to FIFO.
		for (const VkPresentModeKHR presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkSurfaceCapabilitiesKHR VulkanSurface::getCapabilities() const {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, object, &surfaceCapabilities);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface capabilities ([]).", string_VkResult(result));

		return surfaceCapabilities;
	}
}