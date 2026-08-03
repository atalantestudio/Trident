// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Geometry/Plane3D.hpp"
#include "Engine/Build/Cluster.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct GeometryAsset;

	template<>
	struct Descriptor<GeometryAsset> : public Asset {
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

		// NOTE: Hull2DType in 2D, Hull3DType in 3D.
		uint8 hullType;

		/// In 2D, only the first 2 components are used.
		svec3<float32> hullCentroid;

		float32 hullRadius;

		sequence<Edge3D> hull3DEdges;
		sequence<Plane3D> hull3DFaces;
		sequence<uint16> hull3DFaceEdgeIndices;

		/// In 2D, the wireframe is rendered with the `LINE_STRIP` topology.
		/// Geometry hulls are not guaranteed to support wireframe rendering.
		// TODO: Use 16-bit indices for 2D hull wireframes.
		sequence<uint32> hull2DLineIndices;

		/// In 3D, it is rendered with the `LINE_LIST` topology.
		/// Geometry hulls are not guaranteed to support wireframe rendering.
		sequence<uint16> hull3DLineIndices;
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

	struct Hull3DHeader {
		svec3<float32> centroid;
		float32 radius;

		// Total number of edges, including base and twins.
		uint16 edgeCount;

		uint16 faceCount;
		uint16 faceEdgeIndexCount;
		uint16 lineIndexCount;
		Hull3DType hullType;
	};

	struct GeometryAsset : public Asset {
		public:
			explicit GeometryAsset(const Descriptor<GeometryAsset>& descriptor) :
				Asset{descriptor.identifier, descriptor.version},
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
				hull3DEdges(descriptor.hull3DEdges),
				hull3DFaces(descriptor.hull3DFaces),
				hull3DFaceEdgeIndices(descriptor.hull3DFaceEdgeIndices),
				hull2DLineIndices(descriptor.hull2DLineIndices),
				hull3DLineIndices(descriptor.hull3DLineIndices)
			{}

			inline GeometryAssetFlags getFlags() const {
				return flags;
			}

			inline uint32 getPositionCount() const {
				return static_cast<uint32>(vertexPositionComponents.count() / 3);
			}

			inline uint32 getNormalCount() const {
				return static_cast<uint32>(vertexNormalComponents.count() / 3);
			}

			inline uint32 getTextureCoordinateCount() const {
				return static_cast<uint32>(vertexTextureCoordinateComponents.count() / 2);
			}

			inline uint32 getIndexCount() const {
				return static_cast<uint32>(vertexPositionIndices.count());
			}

			inline uint32 getTriangleCount() const {
				return getIndexCount() / 3;
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

			inline uint8 getHullType() const {
				return hullType;
			}

			inline svec3<float32> getHullCentroid() const {
				return hullCentroid;
			}

			inline float32 getHullRadius() const {
				return hullRadius;
			}

			inline const sequence<Edge3D>& getHull3DEdges() const {
				return hull3DEdges;
			}

			inline const sequence<Plane3D>& getHull3DFaces() const {
				return hull3DFaces;
			}

			inline const sequence<uint16>& getHull3DFaceEdgeIndices() const {
				return hull3DFaceEdgeIndices;
			}

			inline const sequence<uint32>& getHull2DLineIndices() const {
				return hull2DLineIndices;
			}

			inline const sequence<uint16>& getHull3DLineIndices() const {
				return hull3DLineIndices;
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
			uint8 hullType;
			svec3<float32> hullCentroid;
			float32 hullRadius;
			sequence<Edge3D> hull3DEdges;
			sequence<Plane3D> hull3DFaces;
			sequence<uint16> hull3DFaceEdgeIndices;
			sequence<uint32> hull2DLineIndices;
			sequence<uint16> hull3DLineIndices;
	};
}