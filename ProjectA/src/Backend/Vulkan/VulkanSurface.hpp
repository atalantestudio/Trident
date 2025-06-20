#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VulkanSurface final : public Surface, public ObjectWrapper<VkSurfaceKHR> {
		public:
			VulkanSurface(VkInstance instance, const Window& window);

			~VulkanSurface() final override;

			virtual ImageFormat getFormat() const final override;

			void initPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, uint32 graphicsQueueFamilyIndex);

			VkSurfaceFormatKHR selectFormat(VkPhysicalDevice physicalDevice) const;

			VkPresentModeKHR selectPresentMode(VkPhysicalDevice physicalDevice, bool vSync) const;

			VkSurfaceCapabilitiesKHR getCapabilities(VkPhysicalDevice physicalDevice) const;

		public:
			uint32 presentQueueFamilyIndex = 0;
			const Window& window;

		private:
			const VkInstance instance;
	};
}