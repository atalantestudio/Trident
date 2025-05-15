#pragma once

namespace ProjectA {
	static VkSampleCountFlagBits getVkSampleCountFlagBits(uint8 sampleCount) {
		if (sampleCount == VK_SAMPLE_COUNT_64_BIT) {
			return VK_SAMPLE_COUNT_64_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_32_BIT) {
			return VK_SAMPLE_COUNT_32_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_16_BIT) {
			return VK_SAMPLE_COUNT_16_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_8_BIT) {
			return VK_SAMPLE_COUNT_8_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_4_BIT) {
			return VK_SAMPLE_COUNT_4_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_2_BIT) {
			return VK_SAMPLE_COUNT_2_BIT;
		}

		if (sampleCount == VK_SAMPLE_COUNT_1_BIT) {
			return VK_SAMPLE_COUNT_1_BIT;
		}

		DEBUG_ASSERT(false);

		return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
	}
}