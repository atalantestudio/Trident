#include "pch.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Backend/Win32/Surface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(VkInstance instance, const VulkanPhysicalDevice& physicalDevice, const Window& window) :
		instance(instance),
		physicalDevice(physicalDevice.object),
		window(window)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result;

		#ifdef PLATFORM_WINDOWS
			result = createVulkanWin32Surface(window, instance, nullptr, &object);
		#else
			#error "Unhandled platform."
		#endif

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface (code: []).", result);

		VkBool32 supported;

		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.object, physicalDevice.graphicsQueueFamilyIndex, object, &supported);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan physical device surface support (code: []).", result);

		if (supported) {
			presentQueueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;
		}

		selectFormat();
		selectPresentMode();

		capabilities = getCapabilities();
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroySurfaceKHR(instance, object, nullptr);
	}

	void VulkanSurface::selectFormat() {
		uint32 formatCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface format count (code: []).", result);

		fchain<VkSurfaceFormatKHR> formats(formatCount);

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, &formats[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface formats (code: []).", result);

		for (const VkSurfaceFormatKHR& format : formats) {
			//if (format.format == VK_FORMAT_B8G8R8A8_SRGB) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM) {
				this->format = format;

				return;
			}
		}

		ENGINE_ASSERT(false, "Could not select a Vulkan surface format.");
	}

	void VulkanSurface::selectPresentMode() {
		uint32 presentModeCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present mode count (code: []).", result);

		fchain<VkPresentModeKHR> presentModes(presentModeCount);

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, &presentModes[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface present modes (code: []).", result);

		// VK_PRESENT_MODE_FIFO_KHR is guaranteed to be always available.
		presentMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const VkPresentModeKHR& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				this->presentMode = presentMode;

				break;
			}
		}
	}

	VkSurfaceCapabilitiesKHR VulkanSurface::getCapabilities() const {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, object, &surfaceCapabilities);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get Vulkan surface capabilities (code: []).", result);

		return surfaceCapabilities;
	}

	ImageFormat VulkanSurface::getFormat() const {
		// TODO
		return ImageFormat::B8G8R8A8_UNORM;
	}
}