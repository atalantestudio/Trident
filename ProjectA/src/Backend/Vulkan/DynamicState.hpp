#pragma once

namespace ProjectA {
	sequence<VkDynamicState> getVkDynamicStates(DynamicState dynamicState) {
		uint32 dynamicStateCount = 0;

		if (dynamicState & DynamicState::VIEWPORT) {
			dynamicStateCount += 1;
		}

		if (dynamicState & DynamicState::SCISSOR) {
			dynamicStateCount += 1;
		}

		if (dynamicState & DynamicState::LINE_WIDTH) {
			dynamicStateCount += 1;
		}

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