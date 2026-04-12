// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Geometry.hpp"

namespace Trident {
	class Geometry3D;

	template<>
	struct Descriptor<Geometry3D> {
		/* sequence<float32> vertexPositionComponents;
		sequence<uint32> vertexPositionIndices;
		sequence<float32> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices; */

		range<uint32, uint32> vertexPositionComponentRange;
		range<uint32, uint32> vertexNormalComponentRange;
		range<uint32, uint32> vertexTextureCoordinateComponentRange;

		uint32 triangleCount;
		uint32 triangleTangentComponentOffset;

		/// Indices for the default visualization mode (TRIANGLE_LIST).
		uint32 vertexPositionIndexOffset;
		uint32 vertexNormalIndexOffset;
		uint32 vertexTextureCoordinateIndexOffset;
	};

	class Geometry3D {
		public:
			explicit Geometry3D() = default;

			explicit Geometry3D(const Descriptor<Geometry3D>& descriptor) :
				vertexPositionComponentRange(descriptor.vertexPositionComponentRange),
				vertexNormalComponentRange(descriptor.vertexNormalComponentRange),
				vertexTextureCoordinateComponentRange(descriptor.vertexTextureCoordinateComponentRange),
				triangleTangentComponentRange({descriptor.triangleTangentComponentOffset, descriptor.triangleCount * 3}),
				vertexPositionIndexRange({descriptor.vertexPositionIndexOffset, descriptor.triangleCount * 3}),
				vertexNormalIndexRange({descriptor.vertexNormalIndexOffset, descriptor.triangleCount * 3}),
				vertexTextureCoordinateIndexRange({descriptor.vertexTextureCoordinateIndexOffset, descriptor.triangleCount * 3})
			{}

			/* explicit Geometry3D(ref<GeometryAsset> geometryAsset) :
				vertexPositionComponents(geometryAsset->getVertexPositionComponents()),
				vertexNormalComponents(geometryAsset->getVertexNormalComponents()),
				vertexTextureCoordinateComponents(geometryAsset->getVertexTextureCoordinateComponents()),
				vertexTangentComponents(geometryAsset->getVertexPositionIndices().count()),
				vertexPositionIndices(geometryAsset->getVertexPositionIndices()),
				vertexNormalIndices(geometryAsset->getVertexNormalIndices()),
				vertexTextureCoordinateIndices(geometryAsset->getVertexTextureCoordinateIndices())
			{}

			inline const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			inline const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			inline const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			inline const sequence<float32>& getVertexTangentComponents() const {
				return vertexTangentComponents;
			}

			inline const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			inline const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			inline const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			} */

			inline range<uint32, uint32> getVertexPositionComponentRange() const {
				return vertexPositionComponentRange;
			}

			inline range<uint32, uint32> getVertexNormalComponentRange() const {
				return vertexNormalComponentRange;
			}

			inline range<uint32, uint32> getVertexTextureCoordinateComponentRange() const {
				return vertexTextureCoordinateComponentRange;
			}

			inline range<uint32, uint32> getTriangleTangentComponentRange() const {
				return triangleTangentComponentRange;
			}

			inline range<uint32, uint32> getVertexPositionIndexRange() const {
				return vertexPositionIndexRange;
			}

			inline range<uint32, uint32> getVertexNormalIndexRange() const {
				return vertexNormalIndexRange;
			}

			inline range<uint32, uint32> getVertexTextureCoordinateIndexRange() const {
				return vertexTextureCoordinateIndexRange;
			}

		protected:
			// TODO: Deprecate and use ranges instead.
			/* sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<float32> vertexTangentComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices; */

			range<uint32, uint32> vertexPositionComponentRange;
			range<uint32, uint32> vertexNormalComponentRange;
			range<uint32, uint32> vertexTextureCoordinateComponentRange;
			range<uint32, uint32> triangleTangentComponentRange;

			/// Indices for the default visualization mode (TRIANGLE_LIST).
			// TODO: Unify the vertex position/normal/texture coordinate indices since they must be the same length.
			range<uint32, uint32> vertexPositionIndexRange;
			range<uint32, uint32> vertexNormalIndexRange;
			range<uint32, uint32> vertexTextureCoordinateIndexRange;
	};
}