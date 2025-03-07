#pragma once

#include "Engine/Renderer/DescriptorSetLayout.hpp"

namespace ProjectA {
	struct PipelineLayoutDescriptor {
		std::vector<DescriptorSetLayout> layouts;
		// TODO: Pass uniform buffers.
	};

	class PipelineLayout {
		public:
			virtual ~PipelineLayout() = default;
	};
}