#pragma once

namespace ProjectA {
	static VkBlendFactor getVkBlendFactor(BlendFactor blendFactor) {
		return static_cast<VkBlendFactor>(blendFactor);
	}
}