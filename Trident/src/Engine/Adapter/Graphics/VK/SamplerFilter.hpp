// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/SamplerFilter.hpp"

namespace Trident::VK {
	inline VkFilter getFilter(SamplerFilter samplerFilter) {
		static constexpr VkFilter FILTERS[] = {
			VK_FILTER_NEAREST,
			VK_FILTER_LINEAR,
		};

		ADAPTER_ASSERT(samplerFilter < SamplerFilter::count, "Invalid sampler filter.");

		return FILTERS[static_cast<uint32>(samplerFilter)];
	}
}