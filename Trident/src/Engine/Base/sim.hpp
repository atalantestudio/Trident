// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// A K-th dimensional simplex.
	template<std::size_t K, typename T>
	struct sim;

	/// A 2-dimensional simplex.
	template<typename T>
	struct sim<2, T> {
		public:
			sim() :
				count(0)
			{}

			sim(vec2<T> a) :
				m{a},
				count(1)
			{}

			sim(vec2<T> a, vec2<T> b) :
				m{a, b},
				count(2)
			{}

			inline vec2<T> operator[](uint8 index) const {
				DEBUG_ASSERT(index < count);

				return m[index];
			}

			inline vec2<T>& operator[](uint8 index) {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		public:
			vec2<T> m[3];
			uint8 count : 2;
	};

	template<typename T>
	using sim2 = sim<2, T>;
}