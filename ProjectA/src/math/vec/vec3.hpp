#pragma once

#include <cmath>
#include <utility>

namespace ProjectA {
	template<typename T>
	class vec3 {
		public:
			T entries[3];

		public:
			vec3() = default;
			vec3(T, T, T);
			vec3(const vec3&);
			vec3(const vec3&&) noexcept;

			const T operator [](unsigned int) const;
			T& operator [](unsigned int);

			T dot(const vec3&) const;

			T length() const;

			vec3& operator =(const vec3&);

			vec3&& operator +(const vec3&) const &;
			vec3&& operator +(const vec3&) &&;

			vec3& operator +=(const vec3&);

			vec3&& operator -() const &;
			vec3&& operator -() &&;
			vec3&& operator -(const vec3&) const &;
			vec3&& operator -(const vec3&) &&;

			vec3& operator -=(const vec3&);

			vec3&& operator *(const vec3&) const &;
			vec3&& operator *(const vec3&) &&;

			vec3& operator *=(const vec3&);

			vec3&& operator /(const vec3&) const &;
			vec3&& operator /(const vec3&) &&;

			vec3& operator /=(const vec3&);

			vec3& normalize();

			vec3&& cross(const vec3&) const &;
			vec3&& cross(const vec3&) &&;
	};
}