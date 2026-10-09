// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, vec2<T> x) {
		stream << x.x << "  " << x.y;

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, vec3<T> x) {
		stream << x.x << "  " << x.y << "  " << x.z;

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, vec4<T> x) {
		stream << x.x << "  " << x.y << "  " << x.z << "  " << x.w;

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, const mat2x2<T>& x) {
		stream << x[0] << '\n' << x[1];

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, const mat3x3<T>& x) {
		stream << x[0] << '\n' << x[1] << '\n' << x[2];

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, const mat4x4<T>& x) {
		stream << x[0] << '\n' << x[1] << '\n' << x[2] << '\n' << x[3];

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator<<(std::ostringstream& stream, quat<T> x) {
		stream << x.w << "  " << x.x << "  " << x.y << "  " << x.z;

		return stream;
	}

	inline std::ostringstream& operator<<(std::ostringstream& stream, __m128 m) {
		#if COMPILER == COMPILER_MSVC
			const std::float_t* buffer = m.m128_f32;
		#else
			std::float_t buffer[4];

			_mm_store_ps(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}

	inline std::ostringstream& operator<<(std::ostringstream& stream, __m128d m) {
		#if COMPILER == COMPILER_MSVC
			const std::double_t* buffer = m.m128d_f64;
		#else
			std::double_t buffer[2];

			_mm_store_pd(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1];

		return stream;
	}

	inline std::ostringstream& operator<<(std::ostringstream& stream, __m256d m) {
		#if COMPILER == COMPILER_MSVC
			const std::double_t* buffer = m.m256d_f64;
		#else
			std::double_t buffer[4];

			_mm256_store_pd(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}
}

namespace scroll {
	using Trident::operator<<;

	template<>
	inline std::string toString(Trident::vec2<std::float_t> x) {
		std::ostringstream stream;
		stream << x;

		return stream.str();
	}

	template<>
	inline std::string toString(Trident::vec3<std::float_t> x) {
		std::ostringstream stream;
		stream << x;

		return stream.str();
	}
}