#pragma once

#include "Backend/Win32/base.hpp"

namespace ProjectA {
	// Creates a Vulkan Win32 surface object.
	static VkResult createVulkanWin32Surface(const Window& window, VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
		VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo{};
		win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		win32SurfaceCreateInfo.pNext = nullptr;
		win32SurfaceCreateInfo.flags = 0;
		win32SurfaceCreateInfo.hinstance = GetInstance();
		win32SurfaceCreateInfo.hwnd = static_cast<const Win32WindowData*>(window.data)->hWnd;

		return vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfo, pAllocator, pSurface);
	}
}