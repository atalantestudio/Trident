#pragma once

#include <iostream>
#include <utility>

namespace ProjectA {
	template<typename T>
	struct vec4 {
		T x, y, z, w;

		vec4() = default;
		vec4(T, T, T, T);
		vec4(const vec4&);
		vec4(const vec4&&);

		const T operator [](unsigned int) const;
		T& operator [](unsigned int);

		vec4& operator =(const vec4&);

		vec4&& operator +(const vec4&) const &;

		vec4&& operator +(const vec4&) &&;

		vec4& operator +=(const vec4&);

		vec4&& operator -() const&;

		vec4&& operator -() &&;

		vec4&& operator -(const vec4&) const &;

		vec4&& operator -(const vec4&) &&;

		vec4& operator -=(const vec4&);

		vec4&& operator *(const vec4&) const &;

		vec4&& operator *(const vec4&) &&;

		vec4& operator *=(const vec4&);

		vec4&& operator /(const vec4&) const &;

		vec4&& operator /(const vec4&) &&;

		vec4& operator /=(const vec4&);

		bool operator ==(const vec4&) const;

		vec4& normalize();

		T dot(const vec4&) const;

		T length() const;
	};

	template<typename T>
	inline std::ostream& operator <<(std::ostream&, const vec4<T>&);
}