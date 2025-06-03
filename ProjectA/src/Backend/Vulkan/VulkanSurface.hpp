#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VulkanSurface : public Surface, public ObjectWrapper<VkSurfaceKHR> {
		public:
			VulkanSurface(VkInstance instance, const Window& window);

			~VulkanSurface();

			void initPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, uint32 graphicsQueueFamilyIndex);

			VkSurfaceFormatKHR selectFormat(VkPhysicalDevice physicalDevice) const;

			VkPresentModeKHR selectPresentMode(VkPhysicalDevice physicalDevice) const;

			VkSurfaceCapabilitiesKHR getCapabilities(VkPhysicalDevice physicalDevice) const;

			virtual ImageFormat getFormat() const final override;

		public:
			uint32 presentQueueFamilyIndex = 0;
			const Window& window;

		private:
			const VkInstance instance;
	};
}