// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"

namespace Trident {
	class Geometry2D;

	template<>
	struct Descriptor<Geometry2D> {
		range<uint32, uint32> vertexPositionComponentRange;
		range<uint32, uint32> vertexTextureCoordinateComponentRange;
		range<uint32, uint32> indexRange;
	};

	class Geometry2D {
		public:
			explicit Geometry2D() = default;

			// TODO: Only used by the asset builder.
			explicit Geometry2D(const Descriptor<Geometry2D>& descriptor) :
				vertexPositionComponentRange(descriptor.vertexPositionComponentRange),
				vertexTextureCoordinateComponentRange(descriptor.vertexTextureCoordinateComponentRange),
				indexRange(descriptor.indexRange)
			{}

			inline range<uint32, uint32> getVertexPositionComponentRange() const {
				return vertexPositionComponentRange;
			}

			inline range<uint32, uint32> getVertexTextureCoordinateComponentRange() const {
				return vertexTextureCoordinateComponentRange;
			}

			inline range<uint32, uint32> getIndexRange() const {
				return indexRange;
			}

		public:
			uint32 hullIndex = max<uint32>();

		private:
			range<uint32, uint32> vertexPositionComponentRange;
			range<uint32, uint32> vertexTextureCoordinateComponentRange;

			/// Indices for the default visualization mode (TRIANGLE_LIST).
			range<uint32, uint32> indexRange;
	};
}