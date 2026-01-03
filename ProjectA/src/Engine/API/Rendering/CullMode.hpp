#pragma once

namespace ProjectA {
	enum class CullMode : uint8 {
		NONE = 0,
		FRONT = 1,
		BACK = 2,
		FRONT_AND_BACK = 3,

		count,
	};
}