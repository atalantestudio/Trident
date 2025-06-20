#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"
#include "Engine/API/Rendering/Sampler.hpp"

namespace ProjectA {
	class VulkanSampler final : public Sampler, public ObjectWrapper<VkSampler> {
		public:
			VulkanSampler(VkDevice device, const SamplerDescriptor& descriptor);

			~VulkanSampler() final override;

		private:
			VkDevice device;
	};
}