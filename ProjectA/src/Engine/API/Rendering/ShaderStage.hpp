#pragma once

namespace ProjectA {
	// TODO: Treat as flag.
	enum class ShaderStage : uint8 {
		VERTEX = 0,
		FRAGMENT = 1,
		COMPUTE = 2,

		__MAX_ENUM,
	};
}