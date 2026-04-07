// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"

namespace Trident {
	class Geometry2D;

	template<>
	struct Descriptor<Geometry2D> {
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> indices;
		uint32 indexOffset;
	};

	class Geometry2D {
		public:
			// TODO: Move inside the asset builder. This geometry must not be created at runtime.
			/// Creates and returns a 2D geometry representing a unit circle.
			static Geometry2D unitCircle(uint32 resolution);

			// TODO: Move inside the asset builder. This geometry must not be created at runtime.
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
			explicit Geometry2D() = default;

			explicit Geometry2D(const Descriptor<Geometry2D>& descriptor) :
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				indices(descriptor.indices),
				indexOffset(descriptor.indexOffset)
			{}

			inline uint32 getVertexCount() const {
				return static_cast<uint32>(vertexPositionComponents.count()) / 2;
			}

			inline const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			inline const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			inline uint32 getIndexOffset() const {
				return indexOffset;
			}

			inline uint32 getIndexCount() const {
				return static_cast<uint32>(indices.count());
			}

			inline const sequence<uint32>& getIndices() const {
				return indices;
			}

		public:
			uint32 hullIndex = max<uint32>();

		private:
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexTextureCoordinateComponents;

			uint32 indexOffset;

			/// Indices for the default (TRIANGLE_LIST) visualization mode.
			sequence<uint32> indices;
	};
}