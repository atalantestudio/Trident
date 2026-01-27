// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace Trident {
	struct PipelineLayoutDescriptor {
		sequence<ref<DescriptorSetLayout>> descriptorSetLayouts;
	};
}