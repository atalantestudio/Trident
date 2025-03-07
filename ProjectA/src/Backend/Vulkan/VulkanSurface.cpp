#include "pch.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(const VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window) :
		instance(instance),
		format(),
		presentMode(),
		presentQueueFamilyIndex(0)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanSurface]");

		// TODO: If no debug messenger, this fails with "Vulkan instance missing VK_KHR_win32_surface extension".
		// TODO: Replace with https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateWin32SurfaceKHR.html.
		glfwCreateWindowSurface(instance, window, nullptr, &object);

		{
			VkBool32 supported;

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

			bool selected = false;

			for (const VkSurfaceFormatKHR& format : formats) {
				//if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
					this->format = format;

					selected = true;

					break;
				//}
			}

			if (!selected) {
				format = formats[0];
			}
		}

		uint32 presentModeCount = 0;

		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.object, object, &presentModeCount, nullptr);

		// Select present mode.
		if (presentModeCount != 0) {
			std::vector<VkPresentModeKHR> presentModes(presentModeCount);

			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.object, object, &presentModeCount, presentModes.data());

			bool selected = false;

			for (const VkPresentModeKHR& presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					this->presentMode = presentMode;

					selected = true;

					break;
				}
			}

			if (!selected) {
				// VK_PRESENT_MODE_FIFO_KHR is guaranteed to be always available.
				presentMode = VK_PRESENT_MODE_FIFO_KHR;
			}
		}
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanSurface]");

		vkDestroySurfaceKHR(instance, object, nullptr);
	}
}