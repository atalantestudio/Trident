// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TSL only supports 32-bit and 64-bit floating-point formats.
	// TODO(graphics/vertex-input-attribute): Implement remaining formats using scalar vectors.
	enum class VertexInputAttributeFormat : uint8 {
		R8_SINT,
		R8_UINT,
		R10G10B10A2_UNORM,
		R16_SINT,
		R16_UINT,
		R32_SINT,
		R32_UINT,
		R32_FLOAT,
		R16G16_UNORM,
		R16G16_FLOAT,
		// R32G32_SINT,
		R32G32_UINT,
		R32G32_FLOAT,
		// R32G32B32_SINT,
		R32G32B32_UINT,
		R32G32B32_FLOAT,
		// R32G32B32A32_SINT,
		R32G32B32A32_UINT,
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

		count,
	};

	template<typename AttributeType>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat();

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<sint8>() {
		return VertexInputAttributeFormat::R8_SINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<uint8>() {
		return VertexInputAttributeFormat::R8_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<sint16>() {
		return VertexInputAttributeFormat::R16_SINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<uint16>() {
		return VertexInputAttributeFormat::R16_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<sint32>() {
		return VertexInputAttributeFormat::R32_SINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<uint32>() {
		return VertexInputAttributeFormat::R32_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<float32>() {
		return VertexInputAttributeFormat::R32_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec2<sint32>>() {
		return VertexInputAttributeFormat::R32G32_SINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<svec2<uint32>>() {
		return VertexInputAttributeFormat::R32G32_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<svec2<float32>>() {
		return VertexInputAttributeFormat::R32G32_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec3<sint32>>() {
		return VertexInputAttributeFormat::R32G32B32_SINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<svec3<uint32>>() {
		return VertexInputAttributeFormat::R32G32B32_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<svec3<float32>>() {
		return VertexInputAttributeFormat::R32G32B32_FLOAT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec3<float32>>() {
		return VertexInputAttributeFormat::R32G32B32_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<sint32>>() {
		return VertexInputAttributeFormat::R32G32B32A32_SINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<uint32>>() {
		return VertexInputAttributeFormat::R32G32B32A32_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<float32>>() {
		return VertexInputAttributeFormat::R32G32B32A32_FLOAT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<sint64>() {
		return VertexInputAttributeFormat::R64_SINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<uint64>() {
		return VertexInputAttributeFormat::R64_UINT;
	}

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<float64>() {
		return VertexInputAttributeFormat::R64_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec2<sint64>>() {
		return VertexInputAttributeFormat::R64G64_SINT;
	} */

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec2<uint64>>() {
		return VertexInputAttributeFormat::R64G64_UINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec2<float64>>() {
		return VertexInputAttributeFormat::R64G64_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec3<sint64>>() {
		return VertexInputAttributeFormat::R64G64B64_SINT;
	} */

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec3<uint64>>() {
		return VertexInputAttributeFormat::R64G64B64_UINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec3<float64>>() {
		return VertexInputAttributeFormat::R64G64B64_FLOAT;
	}

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<sint64>>() {
		return VertexInputAttributeFormat::R64G64B64A64_SINT;
	} */

	/* template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<uint64>>() {
		return VertexInputAttributeFormat::R64G64B64A64_UINT;
	} */

	template<>
	constexpr VertexInputAttributeFormat getVertexInputAttributeFormat<vec4<float64>>() {
		return VertexInputAttributeFormat::R64G64B64A64_FLOAT;
	}
}