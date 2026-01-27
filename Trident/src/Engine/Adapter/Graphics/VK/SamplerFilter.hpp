// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/SamplerFilter.hpp"

namespace Trident::VK {
	inline VkFilter getFilter(SamplerFilter samplerFilter) {
		static constexpr VkFilter vkFilters[] = {
			VK_FILTER_NEAREST,
			VK_FILTER_LINEAR,
		};

		return vkFilters[static_cast<uint8>(samplerFilter)];
	}
}