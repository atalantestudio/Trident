// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Simplex2Vertex {
		/// Support point on A in world space.
		vec2<float32> vA;

		/// Support point on B in world space.
		vec2<float32> vB;

		/// Difference between `vA` and `vB`.
		vec2<float32> v;

		/// Vertex index on A.
		uint32 iA = 0;

		/// Vertex index on B.
		uint32 iB = 0;

		/// Barycentric coordinate of the closest point.
		float32 u = 0.0f;
	};

	class Simplex2 {
		public:
			Simplex2() = default;

			Simplex2(const Simplex2Vertex& firstVertex) :
				vertices{firstVertex},
				count(1)
			{}

			inline const Simplex2Vertex& operator[](uint8 index) const {
				DEBUG_ASSERT(index < 3);

				return vertices[index];
			}

			inline Simplex2Vertex& operator[](uint8 index) {
				DEBUG_ASSERT(index < 3);

				return vertices[index];
			}

		private:
			Simplex2Vertex vertices[3];

		public:
			float32 divisor = 1.0f;
			uint8 count = 0;
	};
}