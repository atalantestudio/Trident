#pragma once

// This order ensures glfwCreateWindowSurface is defined
#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "Engine/ObjectWrapper.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	class VulkanSurface : public ObjectWrapper<VkSurfaceKHR> {
		public:
			VulkanSurface(const VkInstance instance, const VulkanPhysicalDevice& physicalDevice, GLFWwindow* window);

			~VulkanSurface();

		public:
			uint32 presentQueueFamilyIndex;
			VkSurfaceCapabilitiesKHR capabilities;
			VkSurfaceFormatKHR format;
			VkPresentModeKHR presentMode;

		private:
			const VkInstance instance;
	};
}