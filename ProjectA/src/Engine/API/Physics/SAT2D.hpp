#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	struct SAT2DOutput {
		// If the separation is negative, the shapes are intersecting.
		float32 separation;
	};

	struct SAT2DQuery {
		float32 separation;
	};

	struct AxisProjection {
		float32 min;
		float32 max;
	};

	static float32 calculateOverlap(const AxisProjection& rangeA, const AxisProjection& rangeB) {
		return std::max(rangeA.min - rangeB.max, rangeB.min - rangeA.max);
	}

	static vec2<float32> getEdgeNormal(const ref<ConvexHullGeometry2D>& geometry, uint32 edgeVertexOffset) {
		const vec2<float32>& a = geometry->vertices[geometry->indices[edgeVertexOffset + 0]];
		const vec2<float32>& b = geometry->vertices[geometry->indices[edgeVertexOffset + 1]];

		const vec2<float32> ab = b - a;

		vec2<float32> normal = normalized(perpendicular(ab, 1.0f));

		if (dot(a - geometry->centerOfMass, normal) < 0.0f) {
			normal = -normal;
		}

		return normal;
	}

	static AxisProjection project(const PhysicalObjectCopy2D& A, const vec2<float32>& axis) {
		AxisProjection projection;
		projection.min = dot(A.geometry->vertices[0], axis);
		projection.max = projection.min;

		for (uint32 vertexIndex = 1; vertexIndex < A.geometry->vertices.count; vertexIndex += 1) {
			const float32 p = dot(A.geometry->vertices[vertexIndex], axis);

			projection.min = std::min(projection.min, p);
			projection.max = std::max(projection.max, p);
		}

		return projection;
	}

	static SAT2DQuery queryEdgeDirections(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		const ref<ConvexHullGeometry2D>& geometry = std::static_pointer_cast<ConvexHullGeometry2D>(A.geometry);

		SAT2DQuery query;

		for (uint32 edgeIndex = 0; geometry->getEdgeCount(); edgeIndex += 1) {
			const uint32 edgeVertexOffset = edgeIndex * 2;
			const vec2<float32> axisA = getEdgeNormal(geometry, edgeVertexOffset);
			const AxisProjection projectionA = project(A, axisA);
			const AxisProjection projectionB = project(B, axisA);

			const float32 separation = calculateOverlap(projectionA, projectionB);

			if (separation > 0.0f) {
				query.separation = separation;

				return query;
			}

			query.separation = separation;
		}

		return query;
	}

	// https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
	SAT2DOutput SAT2D(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		SAT2DOutput output;

		const SAT2DQuery queryA = queryEdgeDirections(A, B);

		if (queryA.separation > 0.0f) {
			output.separation = queryA.separation;

			return output;
		}

		const SAT2DQuery queryB = queryEdgeDirections(B, A);

		if (queryB.separation > 0.0f) {
			output.separation = queryB.separation;

			return output;
		}

		output.separation = 0.0f;

		return output;
	}
}