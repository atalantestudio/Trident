#pragma once

#include "math/vec/vec3.hpp"

namespace ProjectA {
	template<typename T>
	class mat4x4 {
		public:
			T entries[16];

		public:
			static mat4x4<float> orthographic(float, float, float, float, float, float);
			static mat4x4<float> translate(const vec3<float>&);
			// TODO: Expand to X/Y
			static mat4x4<float> rotateZ(float);
			// TODO: Scale factory

			mat4x4(T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T);
			mat4x4(const mat4x4&);
			mat4x4(const mat4x4&&) noexcept;

			const T& operator [](unsigned int) const;
			T& operator [](unsigned int);

			mat4x4& operator =(const mat4x4&);

			mat4x4&& operator *(const mat4x4&) const &;
			mat4x4&& operator *(const mat4x4&) &&;

			mat4x4& operator *=(const mat4x4&);

			mat4x4& invert();

			mat4x4& transpose();
	};
}