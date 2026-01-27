// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/InputRate.hpp"

namespace Trident::VK {
	inline VkVertexInputRate getVertexInputRate(InputRate inputRate) {
		static constexpr VkVertexInputRate vkVertexInputRates[] = {
			VK_VERTEX_INPUT_RATE_VERTEX,
			VK_VERTEX_INPUT_RATE_INSTANCE,
		};

		return vkVertexInputRates[static_cast<uint8>(inputRate)];
	}
}