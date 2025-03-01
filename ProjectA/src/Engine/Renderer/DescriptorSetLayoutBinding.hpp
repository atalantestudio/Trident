#pragma once

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32 descriptorCount;
		uint32 stageFlags; // From ShaderStage enum
	};
}