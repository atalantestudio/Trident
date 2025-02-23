#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<typename T>
	static uint32 getAttributeGlType();

	template<>
	static uint32 getAttributeGlType<bool>() {
		return 0x8B56;
	}

	template<>
	static uint32 getAttributeGlType<int32>() {
		return 0x1404;
	}

	template<>
	static uint32 getAttributeGlType<vec2<int32>>() {
		return 0x1404;
	}

	template<>
	static uint32 getAttributeGlType<vec3<int32>>() {
		return 0x1404;
	}

	template<>
	static uint32 getAttributeGlType<vec4<int32>>() {
		return 0x1404;
	}

	template<>
	static uint32 getAttributeGlType<float32>() {
		return 0x1406;
	}

	template<>
	static uint32 getAttributeGlType<vec2<float32>>() {
		return 0x1406;
	}

	template<>
	static uint32 getAttributeGlType<vec3<float32>>() {
		return 0x1406;
	}

	template<>
	static uint32 getAttributeGlType<vec4<float32>>() {
		return 0x1406;
	}

	template<>
	static uint32 getAttributeGlType<mat3x3<float32>>() {
		return 0x1406;
	}

	template<>
	static uint32 getAttributeGlType<mat4x4<float32>>() {
		return 0x1406;
	}
}