// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DynamicState.hpp"

namespace Trident::VK {
	inline sequence<VkDynamicState> getDynamicStates(DynamicState dynamicState) {
		const uint32 dynamicStateCount = countSetBits(static_cast<uint32>(dynamicState));

		sequence<VkDynamicState> dynamicStates(dynamicStateCount);
		uint32 dynamicStateIndex = 0;

		if (dynamicState & DynamicState::VIEWPORT) {
			dynamicStates[dynamicStateIndex] = VK_DYNAMIC_STATE_VIEWPORT;
			dynamicStateIndex += 1;
		}

		if (dynamicState & DynamicState::SCISSOR) {
			dynamicStates[dynamicStateIndex] = VK_DYNAMIC_STATE_SCISSOR;
			dynamicStateIndex += 1;
		}

		if (dynamicState & DynamicState::LINE_WIDTH) {
			dynamicStates[dynamicStateIndex] = VK_DYNAMIC_STATE_LINE_WIDTH;
			dynamicStateIndex += 1;
		}

		return dynamicStates;
	}
}