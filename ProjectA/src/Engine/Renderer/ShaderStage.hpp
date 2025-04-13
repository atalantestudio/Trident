#pragma once

namespace ProjectA {
	enum class ShaderStage : uint8 {
		VERTEX = BIT(0),
		FRAGMENT = BIT(1),
		COMPUTE = BIT(2),
	};

	// TODO: Keep as flag since we now have PipelineStage?
	AS_FLAG(ShaderStage);
}