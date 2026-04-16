// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	inline std::ostringstream& operator <<(std::ostringstream& stream, __m128 m) {
		#if COMPILER == COMPILER_MSVC
			const std::float_t* buffer = m.m128_f32;
		#else
			std::float_t buffer[4];

			_mm_store_ps(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}

	inline std::ostringstream& operator <<(std::ostringstream& stream, __m128d m) {
		#if COMPILER == COMPILER_MSVC
			const std::double_t* buffer = m.m128d_f64;
		#else
			std::double_t buffer[2];

			_mm_store_pd(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1];

		return stream;
	}

	inline std::ostringstream& operator <<(std::ostringstream& stream, __m256d m) {
		#if COMPILER == COMPILER_MSVC
			const std::double_t* buffer = m.m256d_f64;
		#else
			std::double_t buffer[4];

			_mm256_store_pd(buffer, m);
		#endif

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}

	template<std::size_t C, typename T>
	inline std::ostringstream& operator <<(std::ostringstream& stream, lyah::vec<C, T> a) {
		for (std::size_t index = 0; index < C - 1; index += 1) {
			stream << a[index] << "  ";
		}

		stream << a[C - 1];

		return stream;
	}

	template<std::size_t R, std::size_t C, typename T>
	inline std::ostringstream& operator <<(std::ostringstream& stream, const lyah::mat<R, C, T>& a) {
		for (std::size_t index = 0; index < R - 1; index += 1) {
			stream << a[index] << '\n';
		}

		stream << a[R - 1];

		return stream;
	}

	template<typename T>
	inline std::ostringstream& operator <<(std::ostringstream& stream, lyah::quat<T> a) {
		stream << a[0] << "  " << a[1] << "  " << a[2] << "  " << a[3];

		return stream;
	}
}

namespace scroll {
	using Trident::operator<<;

	template<>
	inline std::string toString(lyah::vec<2, std::float_t> a) {
		std::ostringstream stream;
		stream << a;

		return stream.str();
	}

	template<>
	inline std::string toString(lyah::vec<3, std::float_t> a) {
		std::ostringstream stream;
		stream << a;

		return stream.str();
	}
}