#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<typename T>
	static uint32 getAttributeSize();

	template<>
	static inline uint32 getAttributeSize<int8>() {
		return sizeof(int8);
	}

	template<>
	static inline uint32 getAttributeSize<uint8>() {
		return sizeof(uint8);
	}

	template<>
	static inline uint32 getAttributeSize<int16>() {
		return sizeof(int16);
	}

	template<>
	static inline uint32 getAttributeSize<uint16>() {
		return sizeof(uint16);
	}

	template<>
	static inline uint32 getAttributeSize<int32>() {
		return sizeof(int32);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<uint32>() {
		//return sizeof(uint32);
		return sizeof(uint32) * 2;
		//return sizeof(uint32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<float32>() {
		//return sizeof(float32);
		return sizeof(float32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<vec2<float32>>() {
		//return sizeof(float32) * 2;
		return sizeof(float32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<vec3<float32>>() {
		//return sizeof(float32) * 3;
		return sizeof(float32) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<vec4<float32>>() {
		return sizeof(float32) * 4;
	}

	template<>
	static inline uint32 getAttributeSize<int64>() {
		return sizeof(int64);
	}

	template<>
	static inline uint32 getAttributeSize<uint64>() {
		return sizeof(uint64);
	}

	template<>
	static inline uint32 getAttributeSize<float64>() {
		return sizeof(float64);
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<vec2<float64>>() {
		//return sizeof(float64) * 2;
		return sizeof(float64) * 4;
	}

	// NOTE: Alignment WITHOUT relaxed block layout.
	template<>
	static inline uint32 getAttributeSize<vec3<float64>>() {
		//return sizeof(float64) * 3;
		return sizeof(float64) * 4;
	}

	template<>
	static inline uint32 getAttributeSize<vec4<float64>>() {
		return sizeof(float64) * 4;
	}
}