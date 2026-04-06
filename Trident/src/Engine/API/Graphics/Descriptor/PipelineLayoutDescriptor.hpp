// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"

namespace Trident {
	struct PipelineLayoutDescriptor {
		sequence<ref<DescriptorSetLayout>> descriptorSetLayouts;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}