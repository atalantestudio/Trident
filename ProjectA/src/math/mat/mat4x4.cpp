#include "pch.hpp"
#include "mat4x4.hpp"

namespace ProjectA {
	mat4x4<float> mat4x4<float>::identity() {
		return {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	}

	/// Document left-handedness
	/// Document parameters
	mat4x4<float> mat4x4<float>::orthographic(float l, float r, float b, float t, float n, float f) {
		float lr = r - l;
		float bt = t - b;
		float nf = f - n;

		return mat4x4(
			2 / lr, 0, 0, -(l + r) / lr,
			0, 2 / bt, 0, -(b + t) / bt,
			0, 0, 2 / nf, -(n + f) / nf,
			0, 0, 0, 1
		);
	}

	mat4x4<float> mat4x4<float>::translate(const vec3<float>& position) {
		return {
			1, 0, 0, position[0],
			0, 1, 0, position[1],
			0, 0, 1, position[2],
			0, 0, 0, 1
		};
	}

	mat4x4<float> mat4x4<float>::rotateZ(float angle) {
		float c = cos(angle);
		float s = sin(angle);

		return {
			c, s, 0, 0,
		   -s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	}

	mat4x4<float> mat4x4<float>::scale(const vec3<float>& scale) {
		return {
			scale[0], 0, 0, 0,
			0, scale[1], 0, 0,
			0, 0, scale[2], 0,
			0, 0, 0, 1
		};
	}

	/// Argument naming (_--)
	/// Spread
	template<typename T>
	mat4x4<T>::mat4x4(
		T _11, T _12, T _13, T _14,
		T _21, T _22, T _23, T _24,
		T _31, T _32, T _33, T _34,
		T _41, T _42, T _43, T _44
	) : entries{
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44
	} {}

	template<typename T>
	mat4x4<T>::mat4x4(const mat4x4& m) : entries{
		m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]
	} {}

	template<typename T>
	mat4x4<T>::mat4x4(const mat4x4&& m) noexcept : entries{
		m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]
	} {}

	template<typename T>
	const T& mat4x4<T>::operator [](unsigned int index) const {
		return entries[index];
	}

	template<typename T>
	T& mat4x4<T>::operator [](unsigned int index) {
		return entries[index];
	}

	template<typename T>
	mat4x4<T>& mat4x4<T>::operator =(const mat4x4& matrix) {
		std::copy(std::begin(matrix.entries), std::end(matrix.entries), entries);

		return *this;
	}

	template<typename T>
	mat4x4<T>&& mat4x4<T>::operator *(const mat4x4& matrix) const & {
		T a11 = entries[0];
		T a12 = entries[1];
		T a13 = entries[2];
		T a14 = entries[3];
		T a21 = entries[4];
		T a22 = entries[5];
		T a23 = entries[6];
		T a24 = entries[7];
		T a31 = entries[8];
		T a32 = entries[9];
		T a33 = entries[10];
		T a34 = entries[11];
		T a41 = entries[12];
		T a42 = entries[13];
		T a43 = entries[14];
		T a44 = entries[15];
		T b11 = matrix[0];
		T b12 = matrix[1];
		T b13 = matrix[2];
		T b14 = matrix[3];
		T b21 = matrix[4];
		T b22 = matrix[5];
		T b23 = matrix[6];
		T b24 = matrix[7];
		T b31 = matrix[8];
		T b32 = matrix[9];
		T b33 = matrix[10];
		T b34 = matrix[11];
		T b41 = matrix[12];
		T b42 = matrix[13];
		T b43 = matrix[14];
		T b44 = matrix[15];

		mat4x4 result(
			a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41,
			a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42,
			a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43,
			a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44,
			a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41,
			a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42,
			a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43,
			a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44,
			a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41,
			a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42,
			a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43,
			a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44,
			a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41,
			a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42,
			a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43,
			a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44
		);

		return std::move(result);
	}

	template<typename T>
	mat4x4<T>&& mat4x4<T>::operator *(const mat4x4& matrix) && {
		T a11 = entries[0];
		T a12 = entries[1];
		T a13 = entries[2];
		T a14 = entries[3];
		T a21 = entries[4];
		T a22 = entries[5];
		T a23 = entries[6];
		T a24 = entries[7];
		T a31 = entries[8];
		T a32 = entries[9];
		T a33 = entries[10];
		T a34 = entries[11];
		T a41 = entries[12];
		T a42 = entries[13];
		T a43 = entries[14];
		T a44 = entries[15];
		T b11 = matrix[0];
		T b12 = matrix[1];
		T b13 = matrix[2];
		T b14 = matrix[3];
		T b21 = matrix[4];
		T b22 = matrix[5];
		T b23 = matrix[6];
		T b24 = matrix[7];
		T b31 = matrix[8];
		T b32 = matrix[9];
		T b33 = matrix[10];
		T b34 = matrix[11];
		T b41 = matrix[12];
		T b42 = matrix[13];
		T b43 = matrix[14];
		T b44 = matrix[15];

		entries[0] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
		entries[1] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
		entries[2] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
		entries[3] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;
		entries[4] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
		entries[5] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
		entries[6] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
		entries[7] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;
		entries[8] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
		entries[9] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
		entries[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
		entries[11] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;
		entries[12] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
		entries[13] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
		entries[14] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
		entries[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

		return std::move(*this);
	}

	template<typename T>
	mat4x4<T>& mat4x4<T>::operator *=(const mat4x4& matrix) {
		T a11 = entries[0];
		T a12 = entries[1];
		T a13 = entries[2];
		T a14 = entries[3];
		T a21 = entries[4];
		T a22 = entries[5];
		T a23 = entries[6];
		T a24 = entries[7];
		T a31 = entries[8];
		T a32 = entries[9];
		T a33 = entries[10];
		T a34 = entries[11];
		T a41 = entries[12];
		T a42 = entries[13];
		T a43 = entries[14];
		T a44 = entries[15];
		T b11 = matrix[0];
		T b12 = matrix[1];
		T b13 = matrix[2];
		T b14 = matrix[3];
		T b21 = matrix[4];
		T b22 = matrix[5];
		T b23 = matrix[6];
		T b24 = matrix[7];
		T b31 = matrix[8];
		T b32 = matrix[9];
		T b33 = matrix[10];
		T b34 = matrix[11];
		T b41 = matrix[12];
		T b42 = matrix[13];
		T b43 = matrix[14];
		T b44 = matrix[15];

		entries[0] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
		entries[1] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
		entries[2] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
		entries[3] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;
		entries[4] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
		entries[5] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
		entries[6] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
		entries[7] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;
		entries[8] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
		entries[9] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
		entries[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
		entries[11] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;
		entries[12] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
		entries[13] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
		entries[14] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
		entries[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

		return *this;
	}

	/// Naming
	template<typename T>
	mat4x4<T>& mat4x4<T>::invert() {
		T m11 = entries[0];
		T m12 = entries[1];
		T m13 = entries[2];
		T m14 = entries[3];
		T m21 = entries[4];
		T m22 = entries[5];
		T m23 = entries[6];
		T m24 = entries[7];
		T m31 = entries[8];
		T m32 = entries[9];
		T m33 = entries[10];
		T m34 = entries[11];
		T m41 = entries[12];
		T m42 = entries[13];
		T m43 = entries[14];
		T m44 = entries[15];
		T A2323 = m33 * m44 - m34 * m43;
		T A1323 = m32 * m44 - m34 * m42;
		T A1223 = m32 * m43 - m33 * m42;
		T A0323 = m31 * m44 - m34 * m41;
		T A0223 = m31 * m43 - m33 * m41;
		T A0123 = m31 * m42 - m32 * m41;
		T A2313 = m23 * m44 - m24 * m43;
		T A1313 = m22 * m44 - m24 * m42;
		T A1213 = m22 * m43 - m23 * m42;
		T A2312 = m23 * m34 - m24 * m33;
		T A1312 = m22 * m34 - m24 * m32;
		T A1212 = m22 * m33 - m23 * m32;
		T A0313 = m21 * m44 - m24 * m41;
		T A0213 = m21 * m43 - m23 * m41;
		T A0312 = m21 * m34 - m24 * m31;
		T A0212 = m21 * m33 - m23 * m31;
		T A0113 = m21 * m42 - m22 * m41;
		T A0112 = m21 * m32 - m22 * m31;
		T d = 1 / (m11 * (m22 * A2323 - m23 * A1323 + m24 * A1223) - m12 * (m21 * A2323 - m23 * A0323 + m24 * A0223) + m13 * (m21 * A1323 - m22 * A0323 + m24 * A0123) - m14 * (m21 * A1223 - m22 * A0223 + m23 * A0123));

		entries[0] = d * (m22 * A2323 - m23 * A1323 + m24 * A1223);
		entries[1] = d * -(m12 * A2323 - m13 * A1323 + m14 * A1223);
		entries[2] = d * (m12 * A2313 - m13 * A1313 + m14 * A1213);
		entries[3] = d * -(m12 * A2312 - m13 * A1312 + m14 * A1212);
		entries[4] = d * -(m21 * A2323 - m23 * A0323 + m24 * A0223);
		entries[5] = d * (m11 * A2323 - m13 * A0323 + m14 * A0223);
		entries[6] = d * -(m11 * A2313 - m13 * A0313 + m14 * A0213);
		entries[7] = d * (m11 * A2312 - m13 * A0312 + m14 * A0212);
		entries[8] = d * (m21 * A1323 - m22 * A0323 + m24 * A0123);
		entries[9] = d * -(m11 * A1323 - m12 * A0323 + m14 * A0123);
		entries[10] = d * (m11 * A1313 - m12 * A0313 + m14 * A0113);
		entries[11] = d * -(m11 * A1312 - m12 * A0312 + m14 * A0112);
		entries[12] = d * -(m21 * A1223 - m22 * A0223 + m23 * A0123);
		entries[13] = d * (m11 * A1223 - m12 * A0223 + m13 * A0123);
		entries[14] = d * -(m11 * A1213 - m12 * A0213 + m13 * A0113);
		entries[15] = d * (m11 * A1212 - m12 * A0212 + m13 * A0112);

		return *this;
	}

	/// Temporary variable naming (_)
	template<typename T>
	mat4x4<T>& mat4x4<T>::transpose() {
		T _ = 0;

		_ = entries[1]; entries[1] = entries[4]; entries[4] = _;
		_ = entries[2]; entries[2] = entries[8]; entries[8] = _;
		_ = entries[3]; entries[3] = entries[12]; entries[12] = _;
		_ = entries[6]; entries[6] = entries[9]; entries[9] = _;
		_ = entries[7]; entries[7] = entries[13]; entries[13] = _;
		_ = entries[11]; entries[11] = entries[14]; entries[14] = _;

		return *this;
	}

	template class mat4x4<float>;
}