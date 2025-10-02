#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace ProjectA {
	struct PipelineLayoutDescriptor {
		sequence<ref<DescriptorSetLayout>> descriptorSetLayouts;
	};
}