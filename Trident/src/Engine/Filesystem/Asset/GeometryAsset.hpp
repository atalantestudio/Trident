// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

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
	};

	struct Geometry2DAssetHeader : public AssetHeader {
		/// If true, the geometry data is followed by a `Hull2DHeader` structure.
		bool hull;

		uint8 _p0;
		uint8 _p1;
		uint8 _p2;

		uint32 vertexCount;
		uint32 indexCount;
	};

	struct Hull2DHeader {
		// TODO: Rename to Hull2DType.
		Hull2DType hullType;

		uint8 _p0;
		uint8 _p1;
		uint8 _p2;

		/// Average vertex.
		svec2<float32> centroid;

		/// Distance from the centroid to the farthest vertex.
		float32 radius;

		uint32 vertexCount;
		uint16 fillIndexCount;

		/// In 2D, the wireframe is rendered with the `LINE_STRIP` topology.
		/// In 3D, it is rendered with the `LINE_LIST` topology.
		/// Geometries are not guaranteed to support wireframe rendering.
		uint16 lineIndexCount;
	};

	class GeometryAsset : public Asset {
		private:
			static constexpr uint32 CLUSTER_INDEX_COUNT = 384;

		public:
			explicit GeometryAsset(const Descriptor<GeometryAsset>& descriptor) :
				Asset(descriptor),
				clusterCount(static_cast<uint32>(ceil(static_cast<float32>(descriptor.vertexPositionIndices.count()) / CLUSTER_INDEX_COUNT))),
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{}

			inline uint32 getClusterCount() const {
				return clusterCount;
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
			uint32 clusterCount;
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;
	};
}