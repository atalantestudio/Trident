// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorType.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32 descriptorCount;
		ShaderStage stageFlags;
	};
}