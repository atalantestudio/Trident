#include "vec3.hpp"

namespace ProjectA {
	template<typename T>
	vec3<T>::vec3(T x, T y, T z) : entries{x, y, z} {}

	template<typename T>
	vec3<T>::vec3(const vec3& vector) : entries{vector[0], vector[1], vector[2]} {}

	template<typename T>
	vec3<T>::vec3(const vec3&& vector) noexcept : entries{vector[0], vector[1], vector[2]} {}

	template<typename T>
	const T vec3<T>::operator [](unsigned int index) const {
		return entries[index];
	}

	template<typename T>
	T& vec3<T>::operator [](unsigned int index) {
		return entries[index];
	}

	template<typename T>
	T vec3<T>::length() const {
		return std::sqrt(dot(*this));
	}

	template<typename T>
	T vec3<T>::dot(const vec3& vector) const {
		return entries[0] * vector[0] + entries[1] * vector[1] + entries[2] * vector[2];
	}

	template<typename T>
	vec3<T>& vec3<T>::operator =(const vec3& vector) {
		entries[0] = vector[0];
		entries[1] = vector[1];
		entries[2] = vector[2];

		return *this;
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator +(const vec3& vector) const & {
		return std::move(vec3(
			entries[0] + vector[0],
			entries[1] + vector[1],
			entries[2] + vector[2]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator +(const vec3& vector) && {
		entries[0] += vector[0];
		entries[1] += vector[1];
		entries[2] += vector[2];

		return std::move(*this);
	}

	template<typename T>
	vec3<T>& vec3<T>::operator +=(const vec3& vector) {
		entries[0] += vector[0];
		entries[1] += vector[1];
		entries[2] += vector[2];

		return *this;
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator -() const & {
		return std::move(vec3(
			-entries[0],
			-entries[1],
			-entries[2]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator -() && {
		entries[0] = -entries[0];
		entries[1] = -entries[1];
		entries[2] = -entries[2];

		return std::move(*this);
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator -(const vec3& vector) const & {
		return std::move(vec3(
			entries[0] - vector[0],
			entries[1] - vector[1],
			entries[2] - vector[2]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator -(const vec3& vector) && {
		entries[0] -= vector[0];
		entries[1] -= vector[1];
		entries[2] -= vector[2];

		return std::move(*this);
	}

	template<typename T>
	vec3<T>& vec3<T>::operator -=(const vec3& vector) {
		entries[0] -= vector[0];
		entries[1] -= vector[1];
		entries[2] -= vector[2];

		return *this;
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator *(const vec3& vector) const & {
		return std::move(vec3(
			entries[0] * vector[0],
			entries[1] * vector[1],
			entries[2] * vector[2]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator *(const vec3& vector) && {
		entries[0] *= vector[0];
		entries[1] *= vector[1];
		entries[2] *= vector[2];

		return std::move(*this);
	}

	template<typename T>
	vec3<T>& vec3<T>::operator *=(const vec3& vector) {
		entries[0] *= vector[0];
		entries[1] *= vector[1];
		entries[2] *= vector[2];

		return *this;
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator /(const vec3& vector) const & {
		return std::move(vec3(
			entries[0] / vector[0],
			entries[1] / vector[1],
			entries[2] / vector[2]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::operator /(const vec3& vector) && {
		entries[0] /= vector[0];
		entries[1] /= vector[1];
		entries[2] /= vector[2];

		return std::move(*this);
	}

	template<typename T>
	vec3<T>& vec3<T>::operator /=(const vec3& vector) {
		entries[0] /= vector[0];
		entries[1] /= vector[1];
		entries[2] /= vector[2];

		return *this;
	}

	template<typename T>
	vec3<T>& vec3<T>::normalize() {
		const T length = this->length();

		if (length == 0) {
			return *this;
		}

		entries[0] /= length;
		entries[1] /= length;
		entries[2] /= length;

		return *this;
	}

	template<typename T>
	vec3<T>&& vec3<T>::cross(const vec3& vector) const & {
		return std::move(vec3(
			entries[1] * vector[2] - entries[2] * vector[1],
			entries[2] * vector[0] - entries[0] * vector[2],
			entries[0] * vector[1] - entries[1] * vector[0]
		));
	}

	template<typename T>
	vec3<T>&& vec3<T>::cross(const vec3& vector) && {
		const T x = entries[1] * vector[2] - entries[2] * vector[1];
		const T y = entries[2] * vector[0] - entries[0] * vector[2];
		const T z = entries[0] * vector[1] - entries[1] * vector[0];

		entries[0] = x;
		entries[1] = y;
		entries[2] = z;

		return std::move(*this);
	}

	template class vec3<float>;
}