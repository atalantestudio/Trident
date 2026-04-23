// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"

namespace Trident {
	struct PipelineLayoutDescriptor {
		sequence<ref<DescriptorSetLayout>> descriptorSetLayouts;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}