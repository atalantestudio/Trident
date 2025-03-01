#include "pch.hpp"

#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(const VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window) :
		instance(instance),
		format(),
		presentMode()
	{
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: If no debug messenger, this fails with "Vulkan instance missing VK_KHR_win32_surface extension".
		glfwCreateWindowSurface(instance, window, nullptr, &object);

		{
			VkBool32 supported = VK_FALSE;

			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.object, physicalDevice.graphicsQueueFamilyIndex, object, &supported);

			if (supported) {
				presentQueueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;
			}
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice.object, object, &capabilities);

		uint32 formatCount = 0;

		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.object, object, &formatCount, nullptr);

		// Select format.
		if (formatCount != 0) {
			std::vector<VkSurfaceFormatKHR> formats(formatCount);

			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.object, object, &formatCount, formats.data());

			bool selectedFormat = false;

			for (const VkSurfaceFormatKHR& format : formats) {
				if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
					this->format = format;

					selectedFormat = true;

					break;
				}
			}

			if (!selectedFormat) {
				format = formats[0];
			}
		}

		uint32 presentModeCount = 0;

		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.object, object, &presentModeCount, nullptr);

		// Select present mode.
		if (presentModeCount != 0) {
			std::vector<VkPresentModeKHR> presentModes(presentModeCount);

			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.object, object, &presentModeCount, presentModes.data());

			bool selectedPresentMode = false;

			for (const VkPresentModeKHR& presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					this->presentMode = presentMode;

					selectedPresentMode = true;

					break;
				}
			}

			if (!selectedPresentMode) {
				// VK_PRESENT_MODE_FIFO_KHR is guaranteed to be always available.
				presentMode = VK_PRESENT_MODE_FIFO_KHR;
			}
		}
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroySurfaceKHR(instance, object, nullptr);
	}
}