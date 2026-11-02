// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Simplex3Vertex {
		/// Support point on A in world space.
		vec3<float32> vA;

		/// Support point on B in world space.
		vec3<float32> vB;

		/// Difference between `vA` and `vB`.
		vec3<float32> v;

		/// Vertex index on A.
		uint32 iA = 0;

		/// Vertex index on B.
		uint32 iB = 0;

		/// Barycentric coordinate of the closest point.
		float32 u = 0.0f;
	};

	class Simplex3 {
		public:
			Simplex3() = default;

			Simplex3(const Simplex3Vertex& firstVertex) :
				vertices{firstVertex},
				count(1)
			{}

			inline const Simplex3Vertex& operator[](uint8 index) const {
				DEBUG_ASSERT(index < 4);

				return vertices[index];
			}

			inline Simplex3Vertex& operator[](uint8 index) {
				DEBUG_ASSERT(index < 4);

				return vertices[index];
			}

		private:
			Simplex3Vertex vertices[4];

		public:
			float32 divisor = 1.0f;
			uint8 count = 0;
	};
}