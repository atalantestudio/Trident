#pragma once

#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VulkanSurface final : public Surface, public ObjectWrapper<VkSurfaceKHR> {
		public:
			explicit VulkanSurface(VkInstance instance, const Window& window);

			~VulkanSurface() final override;

			void setPhysicalDevice(VkPhysicalDevice physicalDevice);

			virtual ImageFormat getFormat() const final override;

			void checkPresentQueueFamilyIndex(uint32 graphicsQueueFamilyIndex);

			VkSurfaceFormatKHR selectFormat() const;

			VkPresentModeKHR selectPresentMode(bool vSync) const;

			VkSurfaceCapabilitiesKHR getCapabilities() const;

		public:
			const Window& window;

		private:
			const VkInstance instance;
			VkPhysicalDevice physicalDevice;
	};
}