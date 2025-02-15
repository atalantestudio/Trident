#pragma once

#include "pch.hpp"

namespace ProjectA {
	template<std::size_t C>
	struct vec_sts {
		float32 m[C];

		vec_sts() = default;

		vec_sts(lyah::vec<C, float32> a) {
			_mm_store_ps(m, a.m);
		}

		vec_sts& operator =(lyah::vec<C, float32> a) {
			_mm_store_ps(m, a.m);

			return *this;
		}
	};
}