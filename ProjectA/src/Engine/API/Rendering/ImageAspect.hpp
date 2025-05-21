#pragma once

namespace ProjectA {
	enum class ImageAspect : uint8 {
		COLOR = BIT(0),
		DEPTH = BIT(1),
	};

	AS_FLAG(ImageAspect);
}