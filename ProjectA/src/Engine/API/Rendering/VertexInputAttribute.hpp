#pragma once

#include "Engine/API/Rendering/VertexInputAttributeType.hpp"

namespace ProjectA {
	// Attributes are never normalized when using the OpenGL graphics adapter.
	struct VertexInputAttribute {
		// Type of the attribute.
		VertexInputAttributeType type;

		// Offset relative to the start of an element in the vertex input binding, in bytes.
		uint32 offset;
	};

	template<typename T>
	static constexpr uint32 getAlignment();

	template<>
	static constexpr uint32 getAlignment<sint8>() {
		return sizeof(sint32);
	}

	template<>
	static constexpr uint32 getAlignment<uint8>() {
		return sizeof(uint32);
	}

	template<>
	static constexpr uint32 getAlignment<sint16>() {
		return sizeof(sint32);
	}

	template<>
	static constexpr uint32 getAlignment<uint16>() {
		return sizeof(uint32);
	}

	template<>
	static constexpr uint32 getAlignment<sint32>() {
		return sizeof(sint32);
	}

	template<>
	static constexpr uint32 getAlignment<uint32>() {
		return sizeof(uint32);
	}

	template<>
	static constexpr uint32 getAlignment<float32>() {
		return sizeof(float32);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec2<sint32>>() {
		return 2 * sizeof(sint32);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec2<uint32>>() {
		return 2 * sizeof(uint32);
	} */

	template<>
	static constexpr uint32 getAlignment<svec2<float32>>() {
		return 2 * sizeof(float32);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec3<sint32>>() {
		return 4 * sizeof(sint32);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec3<uint32>>() {
		return 4 * sizeof(uint32);
	} */

	template<>
	static constexpr uint32 getAlignment<vec3<float32>>() {
		return 4 * sizeof(float32);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec4<sint32>>() {
		return 4 * sizeof(sint32);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec4<uint32>>() {
		return 4 * sizeof(uint32);
	} */

	template<>
	static constexpr uint32 getAlignment<vec4<float32>>() {
		return 4 * sizeof(float32);
	}

	template<>
	static constexpr uint32 getAlignment<sint64>() {
		return sizeof(sint64);
	}

	template<>
	static constexpr uint32 getAlignment<uint64>() {
		return sizeof(uint64);
	}

	template<>
	static constexpr uint32 getAlignment<float64>() {
		return sizeof(float64);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec2<sint64>>() {
		return 2 * sizeof(sint64);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec2<uint64>>() {
		return 2 * sizeof(uint64);
	} */

	template<>
	static constexpr uint32 getAlignment<vec2<float64>>() {
		return 2 * sizeof(float64);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec3<sint64>>() {
		return 4 * sizeof(sint64);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec3<uint64>>() {
		return 4 * sizeof(uint64);
	} */

	template<>
	static constexpr uint32 getAlignment<vec3<float64>>() {
		return 4 * sizeof(float64);
	}

	/* template<>
	static constexpr uint32 getAlignment<vec4<sint64>>() {
		return 4 * sizeof(sint64);
	} */

	/* template<>
	static constexpr uint32 getAlignment<vec4<uint64>>() {
		return 4 * sizeof(uint64);
	} */

	template<>
	static constexpr uint32 getAlignment<vec4<float64>>() {
		return 4 * sizeof(float64);
	}
}