#pragma once

namespace Trident {
	template<typename T, std::enable_if_t<std::is_integral<T>::value, sint32> = 0>
	T random(T min, T max) {
		static std::random_device seeder;
		static std::mt19937 engine(seeder());
		static std::uniform_int_distribution<T> distribution(min, max);

		return distribution(engine);
	}

	template<typename T, std::enable_if_t<std::is_floating_point<T>::value, sint32> = 0>
	T random(T min, T max) {
		static std::random_device seeder;
		static std::mt19937 engine(seeder());
		static std::uniform_real_distribution<T> distribution(min, max);

		return distribution(engine);
	}
}