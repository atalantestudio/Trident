#pragma once

#include <random>

#include "ProjectA.hpp"

class Random {
	private:
		static std::mt19937 mt;
		static std::uniform_int_distribution<std::mt19937::result_type> distribution;

	public:
		static void initialize();

		template<typename T>
		static T get();

		template<>
		static ProjectA::float32 Random::get() {
			return (ProjectA::float32) distribution(mt) / (ProjectA::float32) std::numeric_limits<ProjectA::uint32>::max();
		}
};