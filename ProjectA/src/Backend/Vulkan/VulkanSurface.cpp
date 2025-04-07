#include "pch.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	VulkanSurface::VulkanSurface(VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window) :
		instance(instance),
		physicalDevice(physicalDevice.object)
	{
		PROFILE_SCOPE(FUNC_SIG);

		#ifdef PLATFORM_WINDOWS
			const PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = static_cast<PFN_vkCreateWin32SurfaceKHR>(static_cast<void*>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR")));

			ENGINE_ASSERT(vkCreateWin32SurfaceKHR != nullptr, "Failed to get the address of PFN_vkCreateWin32SurfaceKHR.");

			const HWND hwnd = GetActiveWindow(); 

			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceCreateInfo.pNext = nullptr;
			surfaceCreateInfo.flags = 0;
			surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
			surfaceCreateInfo.hwnd = glfwGetWin32Window(window);

			// TODO: If no debug messenger, this fails with "Vulkan instance missing VK_KHR_win32_surface extension".
			VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &object);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface (code: []).", result);
		#else
			#error "Unhandled platform."
		#endif

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

		bool formatSelected = false;

		for (const VkSurfaceFormatKHR& format : formats) {
			//if (format.format == VK_FORMAT_B8G8R8A8_SRGB) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM) {
				this->format = format;

				formatSelected = true;

				break;
			}
		}

		ENGINE_ASSERT(formatSelected, "Unable to select a Vulkan surface format.");
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
}