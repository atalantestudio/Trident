#pragma once

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include "Engine/Renderer/Context.hpp"

namespace ProjectA {
	class VulkanContext : public Context {
		public:
			VulkanContext(GLFWwindow* window);

		private:
			#ifdef PLATFORM_WINDOWS
				static void checkWindowsVulkanAvailability();
			#endif
	};
}