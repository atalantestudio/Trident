// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"

namespace Trident {
	class Geometry2D;

	template<>
	struct Descriptor<Geometry2D> {
		sequence<svec2<float32>> vertices;
		sequence<uint16> indices;
	};

	class Geometry2D {
		public:
			/// Creates and returns a 2D geometry representing a unit circle.
			static Geometry2D unitCircle(uint32 resolution);

			/// Creates and returns a 2D geometry representing a unit square.
			/// 
			///         0
			///    0 ‒‒‒‒‒‒‒ 1
			///    ¦         ¦
			/// 3  ¦         ¦  1
			///    ¦         ¦
			///    3 ‒‒‒‒‒‒‒ 2
			///         2
			/// 
			static Geometry2D unitSquare();

		public:
			explicit Geometry2D(const Descriptor<Geometry2D>& descriptor);

			inline const sequence<svec2<float32>>& getVertices() const {
				return vertices;
			}

			inline const sequence<uint16>& getIndices() const {
				return indices;
			}

			inline uint32 getHullIndex() const {
				return hullIndex;
			}

		private:
			sequence<svec2<float32>> vertices;

			/// Indices for the default visualization mode (usually with the TRIANGLE_LIST topology).
			sequence<uint16> indices;

			uint32 hullIndex = max<uint32>();
	};
}