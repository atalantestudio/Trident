// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/InputRate.hpp"

namespace Trident::VK {
	inline VkVertexInputRate getVertexInputRate(InputRate inputRate) {
		static constexpr VkVertexInputRate VERTEX_INPUT_RATES[] = {
			VK_VERTEX_INPUT_RATE_VERTEX,
			VK_VERTEX_INPUT_RATE_INSTANCE,
		};

		ADAPTER_ASSERT(inputRate < InputRate::count, "Invalid input rate.");

		return VERTEX_INPUT_RATES[static_cast<uint32>(inputRate)];
	}
}