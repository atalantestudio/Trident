#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VkInstance& instance);

			~VulkanDevice() override;

			ShaderModule createShaderModule() const override;

		private:
			static int32 calculatePhysicalDeviceScore(const VkPhysicalDevice& physicalDevice);
	};
}