// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/FrontFace.hpp"

namespace Trident::VK {
	inline VkFrontFace getFrontFace(FrontFace frontFace) {
		static constexpr VkFrontFace FRONT_FACES[] = {
			VK_FRONT_FACE_CLOCKWISE,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
		};

		ADAPTER_ASSERT(frontFace < FrontFace::count, "Invalid front face.");

		return FRONT_FACES[static_cast<uint32>(frontFace)];
	}
}