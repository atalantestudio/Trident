// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	// TODO: Point hulls need to have 1 vertex for GJK to work.
	inline static const sequence<svec2<float32>> pointHullVertices = {{0.0f, 0.0f}};

	Hull2D Hull2D::point() {
		return Hull2D({
			.type = Hull2DType::POINT,
			.centroid = {0.0_m, 0.0_m},
			.radius = 0.0_m,
			.vertices = pointHullVertices,
			.lineIndexRange = {0, 0},
		});
	}

	Hull2D::Hull2D(const Descriptor<Hull2D>& descriptor) :
		type(descriptor.type),
		centroid(descriptor.centroid),
		radius(descriptor.radius),
		vertices(descriptor.vertices),
		lineIndexRange(descriptor.lineIndexRange)
	{}

	Hull2D::Hull2D(ref<GeometryAsset> geometryAsset, view<svec2<float32>> vertices, uint32 lineIndexOffset) :
		type(geometryAsset->getHullType()),
		centroid(svec2<float32>(geometryAsset->getHullCentroid()[0], geometryAsset->getHullCentroid()[1])),
		radius(geometryAsset->getHullRadius()),
		vertices(vertices),
		lineIndexRange(lineIndexOffset, static_cast<uint32>(geometryAsset->getHullLineIndices().count()))
	{
		if (type == Hull2DType::POLYGON) {
			const uint32 edgeCount = static_cast<uint32>(geometryAsset->getVertexPositionIndices().count()) / 3 * 2;

			edges = sequence<Edge>(edgeCount);

			uint32 edgeIndex = 0;

			while (edgeIndex < edgeCount) {
				edges[edgeIndex] = {edgeIndex, (edgeIndex + 1) % edgeCount};

				edgeIndex += 1;
			}
		}
	}
}