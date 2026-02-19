// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Surface::Surface(const VK::Instance& instance, const Window& window) :
		instance(instance),
		window(window)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkResult result;

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo{};
			win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			win32SurfaceCreateInfo.pNext = nullptr;
			win32SurfaceCreateInfo.flags = 0;
			win32SurfaceCreateInfo.hinstance = GetInstance();
			win32SurfaceCreateInfo.hwnd = static_cast<const Win32WindowData*>(window.data)->hWnd;

			result = vkCreateWin32SurfaceKHR(instance.instance, &win32SurfaceCreateInfo, nullptr, &surface);
		#else
			#error "Unhandled platform."
		#endif

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create surface ([]).", string_VkResult(result));
	}

	VK::Surface::~Surface() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroySurfaceKHR(instance.instance, surface, nullptr);
	}

	void VK::Surface::setPhysicalDevice(const VK::PhysicalDevice& physicalDevice) {
		this->physicalDevice = physicalDevice.physicalDevice;
	}

	void VK::Surface::checkPresentQueueFamilyIndex(uint32 graphicsQueueFamilyIndex) {
		VkBool32 supported;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyIndex, surface, &supported);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface support ([]).", string_VkResult(result));
		ADAPTER_ASSERT(supported == VK_TRUE, "The graphics queue family does not support this surface.");
	}

	ImageFormat VK::Surface::getFormat() const {
		const VkSurfaceFormatKHR surfaceFormat = selectFormat();

		return fromFormat(surfaceFormat.format);
	}

	VkSurfaceFormatKHR VK::Surface::selectFormat() const {
		uint32 formatCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface format count ([]).", string_VkResult(result));

		sequence<VkSurfaceFormatKHR> formats(formatCount);

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, &formats[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface formats ([]).", string_VkResult(result));

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

		ADAPTER_ASSERT(selectedFormat.format != VK_FORMAT_UNDEFINED, "Could not select a surface frormat.");

		return selectedFormat;
	}

	VkPresentModeKHR VK::Surface::selectPresentMode(bool verticalSync) const {
		uint32 presentModeCount = 0;
		VkResult result;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface present mode count ([]).", string_VkResult(result));

		sequence<VkPresentModeKHR> presentModes(presentModeCount);

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, &presentModes[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface present modes ([]).", string_VkResult(result));

		if (!verticalSync) {
			// Try to find IMMEDIATE.
			for (const VkPresentModeKHR presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
					return presentMode;
				}
			}

			ADAPTER_WARNING("V-Sync is disabled but no matching present mode was found. Reverting to FIFO present mode.");

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

	VkSurfaceCapabilitiesKHR VK::Surface::getCapabilities() const {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface capabilities ([]).", string_VkResult(result));

		return surfaceCapabilities;
	}
}