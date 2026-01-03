#pragma once

#include "Engine/API/Rendering/InputRate.hpp"

namespace ProjectA {
	static VkVertexInputRate getVkVertexInputRate(InputRate inputRate) {
		static constexpr VkVertexInputRate vkVertexInputRates[] = {
			VK_VERTEX_INPUT_RATE_VERTEX,
			VK_VERTEX_INPUT_RATE_INSTANCE,
		};

		return vkVertexInputRates[static_cast<uint8>(inputRate)];
	}
}