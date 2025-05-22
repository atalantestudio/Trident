#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VulkanSurface : public Surface, public ObjectWrapper<VkSurfaceKHR> {
		public:
			VulkanSurface(VkInstance instance, const VulkanPhysicalDevice& physicalDevice, const Window& window);

			~VulkanSurface();

			void selectFormat();

			void selectPresentMode();

			VkSurfaceCapabilitiesKHR getCapabilities() const;

			virtual ImageFormat getFormat() const final override;

		public:
			uint32 presentQueueFamilyIndex;
			VkSurfaceFormatKHR format;
			VkPresentModeKHR presentMode;
			const Window& window;

		private:
			const VkInstance instance;
			const VkPhysicalDevice physicalDevice;
			VkSurfaceCapabilitiesKHR capabilities;
	};
}