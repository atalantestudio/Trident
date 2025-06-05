#pragma once

namespace ProjectA {
	enum class ImageLayout : uint8 {
		UNDEFINED = 0,
		TRANSFER_SRC_OPTIMAL = 1,
		TRANSFER_DST_OPTIMAL = 2,
		SHADER_READ_ONLY_OPTIMAL = 3,
		GENERAL = 4,
	};
}