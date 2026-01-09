// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace ProjectA::VK {
	inline VkBlendFactor getBlendFactor(BlendFactor blendFactor) {
		return static_cast<VkBlendFactor>(blendFactor);
	}
}