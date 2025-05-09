#pragma once

namespace ProjectA {
	// TODO: Remove these functions. SIMD types must not directly be stored on the GPU.
	template<typename T>
	static constexpr uint32 getAttributeSize();

	template<>
	static constexpr uint32 getAttributeSize<sint8>() {
		return sizeof(sint8);
	}

	template<>
	static constexpr uint32 getAttributeSize<uint8>() {
		return sizeof(uint8);
	}

	template<>
	static constexpr uint32 getAttributeSize<sint16>() {
		return sizeof(sint16);
	}

	template<>
	static constexpr uint32 getAttributeSize<uint16>() {
		return sizeof(uint16);
	}

	template<>
	static constexpr uint32 getAttributeSize<sint32>() {
		return sizeof(sint32);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<uint32>() {
		//return sizeof(uint32);
		return sizeof(uint32) * 2;
		//return sizeof(uint32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<float32>() {
		//return sizeof(float32);
		return sizeof(float32) * 2;
		//return sizeof(float32) * 4;
	}

	template<>
	static constexpr uint32 getAttributeSize<svec2<float32>>() {
		return sizeof(svec2<float32>);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<vec2<float32>>() {
		//return sizeof(float32) * 2;
		return sizeof(float32) * 4;
	}

	template<>
	static constexpr uint32 getAttributeSize<svec3<float32>>() {
		return sizeof(svec3<float32>);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<vec3<float32>>() {
		//return sizeof(float32) * 3;
		return sizeof(float32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<vec4<float32>>() {
		return sizeof(float32) * 4;
	}

	template<>
	static constexpr uint32 getAttributeSize<sint64>() {
		return sizeof(sint64);
	}

	template<>
	static constexpr uint32 getAttributeSize<uint64>() {
		return sizeof(uint64);
	}

	template<>
	static constexpr uint32 getAttributeSize<float64>() {
		return sizeof(float64);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<vec2<float64>>() {
		//return sizeof(float64) * 2;
		return sizeof(float64) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static constexpr uint32 getAttributeSize<vec3<float64>>() {
		//return sizeof(float64) * 3;
		return sizeof(float64) * 4;
	}

	template<>
	static constexpr uint32 getAttributeSize<vec4<float64>>() {
		return sizeof(float64) * 4;
	}
}