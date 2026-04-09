// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/VertexInputAttributeType.hpp"

namespace Trident {
	struct VertexInputAttribute {
		// Type of the attribute.
		VertexInputAttributeType type;

		// Offset relative to the start of an element in the vertex input binding, in bytes.
		uint32 offset;

		// TODO: Fill in semantic data with info provided by TSL modules.
		// Used by the DX12 graphics adapter.
		view<char8> hlslSemanticName;
		uint32 hlslSemanticIndex;
	};

	template<typename T>
	constexpr uint32 getAlignment();

	template<>
	constexpr uint32 getAlignment<sint8>() {
		return sizeof(sint32);
	}

	template<>
	constexpr uint32 getAlignment<uint8>() {
		return sizeof(uint32);
	}

	template<>
	constexpr uint32 getAlignment<sint16>() {
		return sizeof(sint32);
	}

	template<>
	constexpr uint32 getAlignment<uint16>() {
		return sizeof(uint32);
	}

	template<>
	constexpr uint32 getAlignment<sint32>() {
		return sizeof(sint32);
	}

	template<>
	constexpr uint32 getAlignment<uint32>() {
		return sizeof(uint32);
	}

	template<>
	constexpr uint32 getAlignment<float32>() {
		return sizeof(float32);
	}

	/* template<>
	constexpr uint32 getAlignment<vec2<sint32>>() {
		return 2 * sizeof(sint32);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec2<uint32>>() {
		return 2 * sizeof(uint32);
	} */

	template<>
	constexpr uint32 getAlignment<svec2<float32>>() {
		return 2 * sizeof(float32);
	}

	/* template<>
	constexpr uint32 getAlignment<vec3<sint32>>() {
		return 4 * sizeof(sint32);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec3<uint32>>() {
		return 4 * sizeof(uint32);
	} */

	template<>
	constexpr uint32 getAlignment<svec3<float32>>() {
		return 3 * sizeof(float32);
	}

	template<>
	constexpr uint32 getAlignment<vec3<float32>>() {
		return 4 * sizeof(float32);
	}

	/* template<>
	constexpr uint32 getAlignment<vec4<sint32>>() {
		return 4 * sizeof(sint32);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec4<uint32>>() {
		return 4 * sizeof(uint32);
	} */

	template<>
	constexpr uint32 getAlignment<vec4<float32>>() {
		return 4 * sizeof(float32);
	}

	template<>
	constexpr uint32 getAlignment<sint64>() {
		return sizeof(sint64);
	}

	template<>
	constexpr uint32 getAlignment<uint64>() {
		return sizeof(uint64);
	}

	template<>
	constexpr uint32 getAlignment<float64>() {
		return sizeof(float64);
	}

	/* template<>
	constexpr uint32 getAlignment<vec2<sint64>>() {
		return 2 * sizeof(sint64);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec2<uint64>>() {
		return 2 * sizeof(uint64);
	} */

	template<>
	constexpr uint32 getAlignment<vec2<float64>>() {
		return 2 * sizeof(float64);
	}

	/* template<>
	constexpr uint32 getAlignment<vec3<sint64>>() {
		return 4 * sizeof(sint64);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec3<uint64>>() {
		return 4 * sizeof(uint64);
	} */

	template<>
	constexpr uint32 getAlignment<vec3<float64>>() {
		return 4 * sizeof(float64);
	}

	/* template<>
	constexpr uint32 getAlignment<vec4<sint64>>() {
		return 4 * sizeof(sint64);
	} */

	/* template<>
	constexpr uint32 getAlignment<vec4<uint64>>() {
		return 4 * sizeof(uint64);
	} */

	template<>
	constexpr uint32 getAlignment<vec4<float64>>() {
		return 4 * sizeof(float64);
	}
}