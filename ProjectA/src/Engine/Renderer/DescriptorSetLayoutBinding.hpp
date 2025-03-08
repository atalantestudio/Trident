#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		ShaderStage stageFlags;
		std::vector<ref<Buffer>> buffers;
	};
}