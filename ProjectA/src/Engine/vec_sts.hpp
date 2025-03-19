#pragma once

#include "pch.hpp"

namespace ProjectA {
	// TODO: Temporary polyfill.
	// lyah::vec<C, T> should be capable of storing itself in buffers.
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