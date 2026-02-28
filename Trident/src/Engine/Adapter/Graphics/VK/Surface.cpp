// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	static VkSurfaceFormatKHR selectSurfaceFormat(view<VkSurfaceFormatKHR> surfaceFormats) {
		for (const VkSurfaceFormatKHR surfaceFormat : surfaceFormats) {
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
				return surfaceFormat;
			}
		}

		ADAPTER_ASSERT(false, "Failed to select a surface frormat.");

		return {};
	}

	static VkPresentModeKHR selectPresentMode(view<VkPresentModeKHR> presentModes, bool verticalSync) {
		if (verticalSync) {
			for (const VkPresentModeKHR presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					return presentMode;
				}
			}

			ADAPTER_WARNING("Failed to find MAILBOX present mode. Reverting to FIFO.");

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		for (const VkPresentModeKHR presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				return presentMode;
			}
		}

		ADAPTER_WARNING("Failed to find a matching IMMEDIATE present mode. Reverting to FIFO.");

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VK::Surface::Surface(const Trident::Instance* instance) :
		Trident::Surface(instance)
	{}

	ref<Trident::Surface> VK::createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Instance* _instance = static_cast<const VK::Instance*>(instance);
		const VK::PhysicalDevice* _physicalDevice = std::static_pointer_cast<VK::PhysicalDevice>(physicalDevice).get();
		VkResult result;

		ref<VK::Surface> surface = createRef<VK::Surface>(instance);

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo{};
			win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			win32SurfaceCreateInfo.pNext = nullptr;
			win32SurfaceCreateInfo.flags = 0;
			win32SurfaceCreateInfo.hinstance = GetInstance();
			win32SurfaceCreateInfo.hwnd = static_cast<const Win32WindowData*>(window->data)->hWnd;

			result = _instance->vkCreateWin32SurfaceKHR(_instance->instance, &win32SurfaceCreateInfo, nullptr, &surface->surface);
		#else
			#error "Unhandled platform."
		#endif

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create surface ([]).", string_VkResult(result));

		// Query graphics queue family support.
		{
			VkBool32 supported;

			result = _instance->vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice->physicalDevice, _physicalDevice->graphicsQueueFamilyIndex, surface->surface, &supported);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface support ([]).", string_VkResult(result));
			ADAPTER_ASSERT(supported == VK_TRUE, "The graphics queue family does not support this surface.");
		}

		// Query capabilities.
		{
			result = _instance->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice->physicalDevice, surface->surface, &surface->capabilities);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface capabilities ([]).", string_VkResult(result));
		}

		// Select format.
		{
			uint32 surfaceFormatCount;

			result = _instance->vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice->physicalDevice, surface->surface, &surfaceFormatCount, nullptr);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface format count ([]).", string_VkResult(result));

			sequence<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);

			result = _instance->vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice->physicalDevice, surface->surface, &surfaceFormatCount, &surfaceFormats[0]);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface formats ([]).", string_VkResult(result));

			surface->surfaceFormat = selectSurfaceFormat(surfaceFormats);
			surface->format = fromFormat(surface->surfaceFormat.format);
		}

		// Select present mode.
		{
			uint32 presentModeCount;

			result = _instance->vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice->physicalDevice, surface->surface, &presentModeCount, nullptr);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface present mode count ([]).", string_VkResult(result));

			sequence<VkPresentModeKHR> presentModes(presentModeCount);

			result = _instance->vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice->physicalDevice, surface->surface, &presentModeCount, &presentModes[0]);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get surface present modes ([]).", string_VkResult(result));

			surface->immediatePresentMode = selectPresentMode(presentModes, false);
			surface->synchronizedPresentMode = selectPresentMode(presentModes, true);
		}

		ADAPTER_INFO("Surface format is []/[].", string_VkFormat(surface->surfaceFormat.format), string_VkColorSpaceKHR(surface->surfaceFormat.colorSpace));
		ADAPTER_INFO("Surface present mode for immediate rendering is [].", string_VkPresentModeKHR(surface->immediatePresentMode));
		ADAPTER_INFO("Surface present mode for V-Sync rendering is [].", string_VkPresentModeKHR(surface->synchronizedPresentMode));

		return surface;
	}

	void VK::destroySurface(const Trident::Instance* instance, const Trident::Surface* surface) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Instance* _instance = static_cast<const VK::Instance*>(instance);
		const VK::Surface* _surface = static_cast<const VK::Surface*>(surface);

		_instance->vkDestroySurfaceKHR(_instance->instance, _surface->surface, nullptr);
	}
}