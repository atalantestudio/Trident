#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	struct SAT2DOutput {
		// If the separation is negative, the shapes are intersecting.
		float32 separation;

		uint32 edgeIndex;
		vec2<float32> edgeA1;
		vec2<float32> edgeA2;
		vec2<float32> supportB;
		vec2<float32> normal;
	};

	static vec2<float32> calculateEdgeNormal(const vec2<float32>& a, const vec2<float32>& b) {
		// Assume clockwise order (edge left = outside).
		return perpendicular(normalized(b - a), -1.0f);
	}

	static void queryEdgeDirections(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, SAT2DOutput& output) {
		const ref<ConvexHullGeometry2D>& geometry = std::static_pointer_cast<ConvexHullGeometry2D>(A.geometry);

		// TODO: Expose edges/normals from ConvexHullGeometry.
		for (uint32 edgeIndex = 0, edgeVertexOffset = 0; edgeVertexOffset < geometry->getVertexCount(); edgeIndex += 1, edgeVertexOffset += 1) {
			const vec2<float32>& edge1 = geometry->vertices[(edgeVertexOffset + 0) % geometry->getVertexCount()];
			const vec2<float32>& edge2 = geometry->vertices[(edgeVertexOffset + 1) % geometry->getVertexCount()];
			const vec2<float32> transformedEdge1 = multiplyTransform(edge1, A.transform);
			const vec2<float32> transformedEdge2 = multiplyTransform(edge2, A.transform);
			const vec2<float32> d = calculateEdgeNormal(transformedEdge1, transformedEdge2);
			const vec2<float32> supportB = support(B, -d);

			const float32 separation = signedDistanceLinePoint(transformedEdge1, transformedEdge2, supportB);

			if (separation > output.separation) {
				output.separation = separation;
				output.edgeIndex = edgeIndex;
				output.edgeA1 = transformedEdge1;
				output.edgeA2 = transformedEdge2;
				output.supportB = supportB;
				output.normal = d;
			}
		}
	}

	// Separating Axis Theorem algorithm implementation in 2D.
	// 
	// - Dirk Gregorius - The Separating Axis Test between Convex Polyhedra
	//   https://gdcvault.com/play/1017646/Physics-for-Game-Programmers-The
	//   https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
	// - Erin Catto - Fast and Simple Physics using Sequential Impulses
	//   https://box2d.org/files/ErinCatto_SequentialImpulses_GDC2006.pdf
	// - Tim Wheeler - 2D Collision Detection and Resolution
	//   https://timallanwheeler.com/blog/2024/08/01/2d-collision-detection-and-resolution
	// - William Bittle - SAT (Separating Axis Theorem)
	//   https://dyn4j.org/2010/01/sat
	// 
	// TODO: Basic optimizations:
	//       - Don't test the same separating axis multiple times
	//       - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	//       - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases. But what if the separation axis changes e.g. due to rotation?
	// https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
	static SAT2DOutput SAT2D(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		SAT2DOutput output;
		output.separation = min<float32>();

		queryEdgeDirections(A, B, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(B, A, output);

		if (output.separation > 0.0f) {
			return output;
		}

		return output;
	}
}