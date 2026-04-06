// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/API/Graphics/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	class VK::Surface final : public Trident::Surface {
		public:
			explicit Surface(const Trident::Instance* instance);

		public:
			VkSurfaceKHR surface;
			VkSurfaceCapabilitiesKHR capabilities;
			VkSurfaceFormatKHR surfaceFormat;
			VkPresentModeKHR immediatePresentMode;
			VkPresentModeKHR synchronizedPresentMode;
	};

	namespace VK {
		ref<Trident::Surface> createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window);

		void destroySurface(const Trident::Instance* instance, const Trident::Surface* surface);
	}
}