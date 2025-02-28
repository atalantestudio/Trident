#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			ShaderModule createShaderModule() const override;

		private:
			static const std::vector<const char*> enabledExtensionNames;

			VkQueue graphicsQueue;
			VkQueue presentQueue;
	};
}