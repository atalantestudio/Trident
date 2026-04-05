// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	class GeometryAsset;

	template<>
	struct Descriptor<GeometryAsset> : public AssetDescriptor {
		// 3D vertex components.
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexNormalComponents;
		sequence<float32> vertexTextureCoordinateComponents;

		// Indices arranged in clusters. Not including degenerate indices.
		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;

		bool hasHull;
		svec3<float32> hullCentroid;
		float32 hullRadius;
	};

	struct Geometry2DAssetHeader : public AssetHeader {
		GeometryAssetFlags flags;
		uint32 vertexCount;
		uint32 indexCount;
	};

	struct Hull2DHeader {
		Hull2DType hullType;

		/// Average vertex.
		svec2<float32> centroid;

		/// Distance from the centroid to the farthest vertex.
		float32 radius;

		uint32 vertexCount;

		/// In 2D, the wireframe is rendered with the `LINE_STRIP` topology.
		/// In 3D, it is rendered with the `LINE_LIST` topology.
		/// Geometry hulls are not guaranteed to support wireframe rendering.
		uint16 lineIndexCount;
	};

	class GeometryAsset : public Asset {
		public:
			explicit GeometryAsset(const Descriptor<GeometryAsset>& descriptor) :
				Asset(descriptor),
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{}

			inline uint32 getClusterCount() const {
				// TODO: Use the constant from Build/Clusterizer.hpp.
				constexpr uint32 CLUSTER_INDEX_COUNT = 384;

				return static_cast<uint32>(ceil(static_cast<float32>(vertexPositionIndices.count()) / CLUSTER_INDEX_COUNT));
			}

			inline const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			inline const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			inline const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			inline const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			inline const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			inline const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

		private:
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;
	};
}