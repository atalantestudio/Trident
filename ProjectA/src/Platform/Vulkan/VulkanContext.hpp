#pragma once

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanInstance.hpp"

#include "Engine/Renderer/Context.hpp"

namespace ProjectA {
	class VulkanContext : public Context {
		public:
			VulkanContext(GLFWwindow* window);

			void swapBuffers() const override;

		private:
			static void checkVulkanAvailability();

		private:
			GLFWwindow* window;
			VulkanInstance instance;
	};
}