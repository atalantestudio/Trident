#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Pipeline.hpp"

namespace ProjectA {
	class VulkanPipeline : public Pipeline, public ObjectWrapper<VkPipeline> {
		public:
			explicit VulkanPipeline(const VkPipelineBindPoint bindPoint) :
				bindPoint(bindPoint)
			{}

		public:
			VkPipelineBindPoint bindPoint;
	};
}