#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace ProjectA {
	static VkBlendFactor getVkBlendFactor(BlendFactor blendFactor) {
		return static_cast<VkBlendFactor>(blendFactor);
	}
}