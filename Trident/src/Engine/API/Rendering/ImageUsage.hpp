#pragma once

namespace Trident {
	enum class ImageUsage : uint8 {
		TRANSFER_SRC = BIT(0),
		TRANSFER_DST = BIT(1),
		SAMPLED = BIT(2),
		STORAGE = BIT(3),
		COLOR_ATTACHMENT = BIT(4),
		DEPTH_STENCIL_ATTACHMENT = BIT(5),
		INPUT_ATTACHMENT = BIT(6),
		TRANSIENT_ATTACHMENT = BIT(7),
	};

	AS_FLAG(ImageUsage);
}