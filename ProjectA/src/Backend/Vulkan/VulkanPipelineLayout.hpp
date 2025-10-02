#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace ProjectA {
	class VulkanPipelineLayout final : public PipelineLayout, public ObjectWrapper<VkPipelineLayout> {
		public:
			explicit VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor);

			~VulkanPipelineLayout() final override;

		private:
			const VkDevice device;
	};
}