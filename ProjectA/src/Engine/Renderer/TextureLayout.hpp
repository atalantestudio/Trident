#pragma once

namespace ProjectA {
	enum class TextureLayout : uint8 {
		UNDEFINED = 0,
		TRANSFER_DST_OPTIMAL = 1,
		SHADER_READ_ONLY_OPTIMAL = 2,
	};
}