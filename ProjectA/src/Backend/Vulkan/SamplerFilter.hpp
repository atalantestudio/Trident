#pragma once

namespace ProjectA {
	static VkFilter getVkFilter(SamplerFilter samplerFilter) {
		static constexpr VkFilter vkFilters[] = {
			VK_FILTER_NEAREST,
			VK_FILTER_LINEAR,
		};

		return vkFilters[static_cast<uint8>(samplerFilter)];
	}
}