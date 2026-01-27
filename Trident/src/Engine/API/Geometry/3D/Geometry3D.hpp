// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Geometry3DDescriptor {
		sequence<float32> vertexPositionComponents;
		sequence<uint32> vertexPositionIndices;
		sequence<float32> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices;
	};

	// TODO: Unify the vertex position/normal/texture coordinate indices since they are all the same length.
	class Geometry3D {
		public:
			static constexpr uint32 VERTEX_POSITION_STRIDE = 3;
			static constexpr uint32 VERTEX_NORMAL_STRIDE = 3;
			static constexpr uint32 VERTEX_TEXTURE_COORDINATE_STRIDE = 2;

		public:
			explicit Geometry3D() = default;

			explicit Geometry3D(const Geometry3DDescriptor& descriptor) :
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{}

			explicit Geometry3D(ref<GeometryAsset> geometryAsset) :
				vertexPositionComponents(geometryAsset->getVertexPositionComponents()),
				vertexNormalComponents(geometryAsset->getVertexNormalComponents()),
				vertexTextureCoordinateComponents(geometryAsset->getVertexTextureCoordinateComponents()),
				vertexPositionIndices(geometryAsset->getVertexPositionIndices()),
				vertexNormalIndices(geometryAsset->getVertexNormalIndices()),
				vertexTextureCoordinateIndices(geometryAsset->getVertexTextureCoordinateIndices())
			{}

			const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

			uint32 getTriangleCount() const {
				return static_cast<uint32>(vertexPositionIndices.count()) / 3;
			}

		protected:
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;

			// TODO: Store cluster count?
	};
}