#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<std::size_t C>
	struct vec_sts {
		float32 m[C];

		vec_sts() = default;

		template<std::size_t C>
		vec_sts(vec<C, float32> a) {
			_mm_store_ps(m, a.m);
		}

		template<std::size_t C>
		vec_sts& operator =(vec<C, float32> a) {
			_mm_store_ps(m, a.m);

			return *this;
		}
	};
}