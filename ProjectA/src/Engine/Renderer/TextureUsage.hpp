#pragma once

namespace ProjectA {
	enum class TextureUsage : uint8 {
		TRANSFER_SRC = 0,
		TRANSFER_DST = 1,
		SAMPLED = 2,
		STORAGE = 3,
		COLOR_ATTACHMENT = 4,
		DEPTH_STENCIL_ATTACHMENT = 5,
	};
}