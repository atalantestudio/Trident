#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Sampler.hpp"

namespace ProjectA {
	class VulkanSampler : public Sampler, public ObjectWrapper<VkSampler> {
		public:
			VulkanSampler(VkDevice device, const SamplerDescriptor& descriptor);

			~VulkanSampler();

		private:
			VkDevice device;
	};
}