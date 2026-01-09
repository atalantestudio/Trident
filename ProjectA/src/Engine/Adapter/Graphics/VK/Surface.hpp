// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VK::Surface final : public ProjectA::Surface {
		public:
			explicit Surface(const Window& window, const VK::Instance& instance);

			~Surface() final override;

			void setPhysicalDevice(const VK::PhysicalDevice& physicalDevice);

			virtual ImageFormat getFormat() const final override;

			void checkPresentQueueFamilyIndex(uint32 graphicsQueueFamilyIndex);

			VkSurfaceFormatKHR selectFormat() const;

			VkPresentModeKHR selectPresentMode(bool vSync) const;

			VkSurfaceCapabilitiesKHR getCapabilities() const;

		public:
			VkSurfaceKHR surface;
			const Window& window;

		private:
			const VK::Instance& instance;
			VkPhysicalDevice physicalDevice;
	};
}