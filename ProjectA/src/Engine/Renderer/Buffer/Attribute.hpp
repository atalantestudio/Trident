#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<typename T>
	static uint32 getAttributeSize();

	template<>
	static inline uint32 getAttributeSize<int32>() {
		return sizeof(int32);
	}

	template<>
	static inline uint32 getAttributeSize<float32>() {
		return sizeof(float32);
	}

	template<>
	static inline uint32 getAttributeSize<vec2<float32>>() {
		return sizeof(float32) * 2;
	}

	template<>
	static inline uint32 getAttributeSize<vec3<float32>>() {
		return sizeof(float32) * 3;
	}

	template<>
	static inline uint32 getAttributeSize<vec4<float32>>() {
		return sizeof(float32) * 4;
	}
}