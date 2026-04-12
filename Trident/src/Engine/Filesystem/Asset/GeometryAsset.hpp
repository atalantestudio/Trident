// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/Build/Cluster.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	class GeometryAsset;

	template<>
	struct Descriptor<GeometryAsset> : public AssetDescriptor {
		GeometryAssetFlags flags;

		// Packed vertices.
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexNormalComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<float32> triangleTangentComponents;

		// The index count is the same for positions, normals and texture coordinates.
		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;

		// TODO: Add support for 3D hulls.
		Hull2DType hullType;

		/// In 2D, only the first 2 components are used.
		svec3<float32> hullCentroid;

		float32 hullRadius;

		/// In 2D, the wireframe is rendered with the `LINE_STRIP` topology.
		/// In 3D, it is rendered with the `LINE_LIST` topology.
		/// Geometry hulls are not guaranteed to support wireframe rendering.
		sequence<uint32> hullLineIndices;
	};

	struct GeometryAsset2DHeader : public AssetHeader {
		GeometryAssetFlags flags;
		uint32 vertexCount;
		uint32 indexCount;
	};

	struct Geometry3DAssetHeader : public AssetHeader {
		GeometryAssetFlags flags;
		uint32 vertexPositionCount;
		uint32 vertexNormalCount;
		uint32 vertexTextureCoordinateCount;
		uint32 triangleTangentCount;

		// The index count is the same for positions, normals and texture coordinates.
		uint32 indexCount;
	};

	struct Hull2DHeader {
		Hull2DType hullType;
		svec2<float32> centroid;
		float32 radius;
		uint32 lineIndexCount;
	};

	class GeometryAsset : public Asset {
		public:
			explicit GeometryAsset(const Descriptor<GeometryAsset>& descriptor) :
				Asset(descriptor),
				flags(descriptor.flags),
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				triangleTangentComponents(descriptor.triangleTangentComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices),
				hullType(descriptor.hullType),
				hullCentroid(descriptor.hullCentroid),
				hullRadius(descriptor.hullRadius),
				hullLineIndices(descriptor.hullLineIndices)
			{}

			inline GeometryAssetFlags getFlags() const {
				return flags;
			}

			inline uint32 getClusterCount() const {
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

			inline const sequence<float32>& getTriangleTangentComponents() const {
				return triangleTangentComponents;
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

			inline Hull2DType getHullType() const {
				return hullType;
			}

			inline svec3<float32> getHullCentroid() const {
				return hullCentroid;
			}

			inline float32 getHullRadius() const {
				return hullRadius;
			}

			inline const sequence<uint32>& getHullLineIndices() const {
				return hullLineIndices;
			}

		private:
			GeometryAssetFlags flags;
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<float32> triangleTangentComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;
			Hull2DType hullType;
			svec3<float32> hullCentroid;
			float32 hullRadius;
			sequence<uint32> hullLineIndices;
	};
}