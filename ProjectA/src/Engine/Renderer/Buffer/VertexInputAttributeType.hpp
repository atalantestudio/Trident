#pragma once

#include "pch.hpp"

namespace ProjectA {
	// TODO: Implement remaining types with the next version of Lyah.
	enum class VertexInputAttributeType : uint32 {
		R8_SINT,
		R8_UINT,
		R16_SINT,
		R16_UINT,
		R32_SINT,
		R32_UINT,
		R32_FLOAT,
		// R32G32_SINT,
		// R32G32_UINT,
		R32G32_FLOAT,
		// R32G32B32_SINT,
		// R32G32B32_UINT,
		R32G32B32_FLOAT,
		// R32G32B32A32_SINT,
		// R32G32B32A32_UINT,
		R32G32B32A32_FLOAT,
		R64_SINT,
		R64_UINT,
		R64_FLOAT,
		// R64G64_SINT,
		// R64G64_UINT,
		R64G64_FLOAT,
		// R64G64B64_SINT,
		// R64G64B64_UINT,
		R64G64B64_FLOAT,
		// R64G64B64A64_SINT,
		// R64G64B64A64_UINT,
		R64G64B64A64_FLOAT,
	};

	template<typename T>
	static VertexInputAttributeType getVertexInputAttributeType();

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<int8>() {
		return VertexInputAttributeType::R8_SINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<uint8>() {
		return VertexInputAttributeType::R8_UINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<int16>() {
		return VertexInputAttributeType::R16_SINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<uint16>() {
		return VertexInputAttributeType::R16_UINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<int32>() {
		return VertexInputAttributeType::R32_SINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<uint32>() {
		return VertexInputAttributeType::R32_UINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<float32>() {
		return VertexInputAttributeType::R32_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<int32>>() {
		return VertexInputAttributeType::R32G32_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<uint32>>() {
		return VertexInputAttributeType::R32G32_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<float32>>() {
		return VertexInputAttributeType::R32G32_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<int32>>() {
		return VertexInputAttributeType::R32G32B32_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<uint32>>() {
		return VertexInputAttributeType::R32G32B32_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<float32>>() {
		return VertexInputAttributeType::R32G32B32_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<int32>>() {
		return VertexInputAttributeType::R32G32B32A32_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<uint32>>() {
		return VertexInputAttributeType::R32G32B32A32_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<float32>>() {
		return VertexInputAttributeType::R32G32B32A32_FLOAT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<int64>() {
		return VertexInputAttributeType::R64_SINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<uint64>() {
		return VertexInputAttributeType::R64_UINT;
	}

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<float64>() {
		return VertexInputAttributeType::R64_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<int64>>() {
		return VertexInputAttributeType::R64G64_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<uint64>>() {
		return VertexInputAttributeType::R64G64_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec2<float64>>() {
		return VertexInputAttributeType::R64G64_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<int64>>() {
		return VertexInputAttributeType::R64G64B64_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<uint64>>() {
		return VertexInputAttributeType::R64G64B64_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec3<float64>>() {
		return VertexInputAttributeType::R64G64B64_FLOAT;
	}

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<int64>>() {
		return VertexInputAttributeType::R64G64B64A64_SINT;
	} */

	/* template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<uint64>>() {
		return VertexInputAttributeType::R64G64B64A64_UINT;
	} */

	template<>
	static inline VertexInputAttributeType getVertexInputAttributeType<vec4<float64>>() {
		return VertexInputAttributeType::R64G64B64A64_FLOAT;
	}
}