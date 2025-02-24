#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<typename T>
	static uint32 getAttributeSize();

	template<>
	static uint32 getAttributeSize<bool>() {
		return sizeof(bool);
	}

	template<>
	static uint32 getAttributeSize<int32>() {
		return sizeof(int32);
	}

	template<>
	static uint32 getAttributeSize<vec2<int32>>() {
		return sizeof(int32) * 2;
	}

	template<>
	static uint32 getAttributeSize<vec3<int32>>() {
		return sizeof(int32) * 3;
	}

	template<>
	static uint32 getAttributeSize<vec4<int32>>() {
		return sizeof(int32) * 4;
	}

	template<>
	static uint32 getAttributeSize<float32>() {
		return sizeof(float32);
	}

	template<>
	static uint32 getAttributeSize<vec2<float32>>() {
		return sizeof(float32) * 2;
	}

	template<>
	static uint32 getAttributeSize<vec3<float32>>() {
		return sizeof(float32) * 3;
	}

	template<>
	static uint32 getAttributeSize<vec4<float32>>() {
		return sizeof(float32) * 4;
	}

	template<>
	static uint32 getAttributeSize<mat3x3<float32>>() {
		return sizeof(float32) * 3; // 3 * vec3
	}

	template<>
	static uint32 getAttributeSize<mat4x4<float32>>() {
		return sizeof(float32) * 4; // 4 * vec4
	}

	template<typename T>
	static uint32 getAttributeComponentCount();

	template<>
	static uint32 getAttributeComponentCount<bool>() {
		return 1;
	}

	template<>
	static uint32 getAttributeComponentCount<int32>() {
		return 1;
	}

	template<>
	static uint32 getAttributeComponentCount<vec2<int32>>() {
		return 2;
	}

	template<>
	static uint32 getAttributeComponentCount<vec3<int32>>() {
		return 3;
	}

	template<>
	static uint32 getAttributeComponentCount<vec4<int32>>() {
		return 4;
	}

	template<>
	static uint32 getAttributeComponentCount<float32>() {
		return 1;
	}

	template<>
	static uint32 getAttributeComponentCount<vec2<float32>>() {
		return 2;
	}

	template<>
	static uint32 getAttributeComponentCount<vec3<float32>>() {
		return 3;
	}

	template<>
	static uint32 getAttributeComponentCount<vec4<float32>>() {
		return 4;
	}

	template<>
	static uint32 getAttributeComponentCount<mat3x3<float32>>() {
		return 3 * 3;
	}

	template<>
	static uint32 getAttributeComponentCount<mat4x4<float32>>() {
		return 4 * 4;
	}
}