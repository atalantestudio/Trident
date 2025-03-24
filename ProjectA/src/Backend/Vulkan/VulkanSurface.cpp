#include "pch.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window) :
		instance(instance),
		physicalDevice(physicalDevice.object)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanSurface]");

		// TODO: If no debug messenger, this fails with "Vulkan instance missing VK_KHR_win32_surface extension".
		// TODO: Replace with https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateWin32SurfaceKHR.html
		// or its Linux variant.
		VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &object);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create surface.");
		}

		VkBool32 supported;

		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.object, physicalDevice.graphicsQueueFamilyIndex, object, &supported);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to get surface support.");
		}

		if (supported) {
			presentQueueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;
		}

		update();
	}

	VulkanSurface::~VulkanSurface() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanSurface]");

		vkDestroySurfaceKHR(instance, object, nullptr);
	}

	void VulkanSurface::update() {
		VkResult result;

		// Get capabilities.
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, object, &capabilities);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to get surface capabilities.");
		}

		uint32 formatCount = 0;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, nullptr);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to get surface format count.");
		}

		// Select format.
		if (formatCount > 0) {
			chain<VkSurfaceFormatKHR> formats(formatCount);

			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, object, &formatCount, formats.data());

			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to get surface formats.");
			}

			bool selected = false;

			for (const VkSurfaceFormatKHR& format : formats) {
				if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
					this->format = format;

					selected = true;

					break;
				}
			}

			if (!selected) {
				format = formats[0];
			}
		}

		uint32 presentModeCount = 0;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, nullptr);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to get surface present mode count.");
		}

		// Select present mode.
		if (presentModeCount > 0) {
			chain<VkPresentModeKHR> presentModes(presentModeCount);

			result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, object, &presentModeCount, presentModes.data());

			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to get surface present modes.");
			}

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
}