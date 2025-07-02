#pragma once

namespace ProjectA {
	// Trims whitespace - white lines not included.
	inline std::string trim(const std::string& string) {
		const std::size_t firstNotSpace = string.find_first_not_of(' ');

		if (firstNotSpace == std::string::npos) {
			return string;
		}

		const std::size_t lastNotSpace = string.find_last_not_of(' ');

		return string.substr(firstNotSpace, (lastNotSpace - firstNotSpace + 1));
	}

	inline std::ostringstream& operator <<(std::ostringstream& stream, __m128 m) {
		const std::float_t* buffer = m.m128_f32;

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}

	inline std::ostringstream& operator <<(std::ostringstream& stream, __m128d m) {
		const std::double_t* buffer = m.m128d_f64;

		stream << buffer[0] << "  " << buffer[1];

		return stream;
	}

	inline std::ostringstream& operator <<(std::ostringstream& stream, __m256d m) {
		const std::double_t* buffer = m.m256d_f64;

		stream << buffer[0] << "  " << buffer[1] << "  " << buffer[2] << "  " << buffer[3];

		return stream;
	}

	template<std::size_t C, typename T>
	inline std::ostringstream& operator <<(std::ostringstream& stream, const lyah::vec<C, T>& a) {
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
	inline std::ostringstream& operator <<(std::ostringstream& stream, const lyah::quat<T>& a) {
		stream << a[0] << "  " << a[1] << "  " << a[2] << "  " << a[3];

		return stream;
	}
}