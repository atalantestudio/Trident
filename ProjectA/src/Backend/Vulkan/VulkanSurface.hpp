#pragma once

#include <vulkan/vulkan.h>

#ifdef PLATFORM_WINDOWS
	#include <vulkan/vulkan_win32.h>

	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <glfw/glfw3.h>

#ifdef PLATFORM_WINDOWS
	#include <glfw/glfw3native.h>
#endif

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Surface.hpp"

namespace ProjectA {
	class VulkanSurface : public Surface, public ObjectWrapper<VkSurfaceKHR> {
		public:
			VulkanSurface(VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window);

			~VulkanSurface();

			void selectFormat();

			void selectPresentMode();

			VkSurfaceCapabilitiesKHR getCapabilities() const;

		public:
			uint32 presentQueueFamilyIndex;
			VkSurfaceFormatKHR format;
			VkPresentModeKHR presentMode;

		private:
			const VkInstance instance;
			const VkPhysicalDevice physicalDevice;
			VkSurfaceCapabilitiesKHR capabilities;
	};
}