#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace ProjectA {
	struct PipelineLayoutDescriptor {
		std::vector<DescriptorSetLayout> layouts;
	};
}