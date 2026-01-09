// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/FrontFace.hpp"

namespace ProjectA::VK {
	inline VkFrontFace getFrontFace(FrontFace frontFace) {
		static constexpr VkFrontFace vkFrontFaces[] = {
			VK_FRONT_FACE_CLOCKWISE,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
		};

		return vkFrontFaces[static_cast<uint8>(frontFace)];
	}
}