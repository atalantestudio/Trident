#pragma once

namespace ProjectA {
	chain<VkDynamicState> getVkDynamicStates(DynamicState dynamicState) {
		chain<VkDynamicState> dynamicStates;

		if (dynamicState & DynamicState::VIEWPORT) {
			dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		}

		if (dynamicState & DynamicState::SCISSOR) {
			dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		}

		if (dynamicState & DynamicState::LINE_WIDTH) {
			dynamicStates.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		}

		return dynamicStates;
	}
}