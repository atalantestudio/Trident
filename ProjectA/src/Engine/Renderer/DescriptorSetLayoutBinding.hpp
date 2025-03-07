#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32 stageFlags; // From ShaderStage enum
		std::vector<ref<Buffer>> buffers;
	};
}