#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace Trident {
	struct PipelineLayoutDescriptor {
		sequence<ref<DescriptorSetLayout>> descriptorSetLayouts;
	};
}