/* #pragma once

#include <random>

#include "Trident.hpp"

class Random {
	private:
		static std::mt19937 mt;
		static std::uniform_int_distribution<std::mt19937::result_type> distribution;

	public:
		static void initialize();

		template<typename T>
		static T get();

		template<>
		static Trident::float32 Random::get() {
			return static_cast<Trident::float32>(distribution(mt)) / Trident::max<Trident::uint32>();
		}
}; */