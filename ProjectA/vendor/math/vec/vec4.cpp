#include "vec4.hpp"

namespace ProjectA {
	template<typename T>
	vec4<T>::vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	template<typename T>
	vec4<T>::vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	template<typename T>
	vec4<T>::vec4(const vec4&& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	template<typename T>
	vec4<T>& vec4<T>::operator =(const vec4& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator +(const vec4& v) const& {
		return std::move(vec4(x + v.x, y + v.y, z + v.z, w + v.w));
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator +(const vec4& v)&& {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return std::move(*this);
	}

	template<typename T>
	vec4<T>& vec4<T>::operator +=(const vec4& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator -() const& {
		return std::move(vec4(-x, -y, -z, -w));
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator -()&& {
		x = -x;
		y = -y;
		z = -z;
		w = -w;

		return std::move(*this);
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator -(const vec4& v) const& {
		return std::move(vec4(x - v.x, y - v.y, z - v.z, w - v.w));
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator -(const vec4& v)&& {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return std::move(*this);
	}

	template<typename T>
	vec4<T>& vec4<T>::operator -=(const vec4& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator *(const vec4& v) const& {
		return std::move(vec4(x * v.x, y * v.y, z * v.z, w * v.w));
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator *(const vec4& v)&& {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return std::move(*this);
	}

	template<typename T>
	vec4<T>& vec4<T>::operator *=(const vec4& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator /(const vec4& v) const& {
		return std::move(vec4(x / v.x, y / v.y, z / v.z, w / v.w));
	}

	template<typename T>
	vec4<T>&& vec4<T>::operator /(const vec4& v)&& {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;

		return std::move(*this);
	}

	template<typename T>
	vec4<T>& vec4<T>::operator /=(const vec4& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;

		return *this;
	}

	template<typename T>
	bool vec4<T>::operator ==(const vec4& v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	template<typename T>
	vec4<T>& vec4<T>::normalize() {
		const T l = length();

		if (l == 0) {
			return *this;
		}

		x /= l;
		y /= l;
		z /= l;
		w /= l;

		return *this;
	}

	template<typename T>
	T vec4<T>::dot(const vec4& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	template<typename T>
	T vec4<T>::length() const {
		return sqrt(dot(*this));
	}

	template<typename T>
	inline std::ostream& operator <<(std::ostream& stream, const vec4<T>& vector) {
		return stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
	}

	template struct vec4<float>;
}