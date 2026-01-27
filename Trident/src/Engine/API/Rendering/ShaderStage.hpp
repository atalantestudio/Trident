#pragma once

namespace Trident {
	enum class ShaderStage : uint8 {
		VERTEX = BIT(0),
		FRAGMENT = BIT(1),
		COMPUTE = BIT(2),
	};

	AS_FLAG(ShaderStage);
}