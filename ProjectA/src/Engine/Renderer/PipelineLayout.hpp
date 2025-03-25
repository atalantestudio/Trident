#pragma once

#include "Engine/Renderer/DescriptorSetLayout.hpp"

namespace ProjectA {
	struct PipelineLayoutDescriptor {
		chain<DescriptorSetLayout> layouts;
	};

	class PipelineLayout {
		public:
			virtual ~PipelineLayout() = default;
	};
}