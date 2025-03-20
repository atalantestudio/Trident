#pragma once

#include "Engine/Renderer/DescriptorSetLayout.hpp"

namespace ProjectA {
	struct PipelineLayoutDescriptor {
		std::vector<DescriptorSetLayout> layouts;
	};

	class PipelineLayout {
		public:
			virtual ~PipelineLayout() = default;
	};
}