// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayoutBinding.hpp"

namespace Trident {
	struct DescriptorSetLayoutDescriptor {
		view<DescriptorSetLayoutBinding> bindings;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}