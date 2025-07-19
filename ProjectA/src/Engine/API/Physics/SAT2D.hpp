#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	struct SAT2DOutput {
		// A separation less than or equal to 0 indicates overlap between the shapes.
		float32 separation;

		vec2<float32> normal;
		vec2<float32> edgeA1;
		vec2<float32> edgeA2;
		vec2<float32> supportB;
		Feature2D closestFeatureA;
		Feature2D closestFeatureB;
		const PhysicalObjectCopy2D* incidentObject;
	};

	static void queryEdgeDirections(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, SAT2DOutput& output) {
		const ref<ConvexHullGeometry2D>& geometry = std::static_pointer_cast<ConvexHullGeometry2D>(A.geometry);

		// TODO: Expose edges/normals from ConvexHullGeometry.
		for (uint32 edgeVertexOffset = 0; edgeVertexOffset < geometry->getVertexCount(); edgeVertexOffset += 1) {
			const vec2<float32> a = multiplyTransform(geometry->vertices[(edgeVertexOffset + 0) % geometry->getVertexCount()], A.transform);
			const vec2<float32> b = multiplyTransform(geometry->vertices[(edgeVertexOffset + 1) % geometry->getVertexCount()], A.transform);
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = support(B, -d);

			const float32 separation = signedDistanceLinePoint(a, b, s);

			if (separation > output.separation) {
				output.separation = separation;
				output.supportB = s;
				output.edgeA1 = a;
				output.edgeA2 = b;
				output.normal = d;
				output.incidentObject = &B;
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

		const SAT2DOutput outputA = output;

		queryEdgeDirections(B, A, output);

		if (output.separation > 0.0f) {
			return output;
		}

		if (outputA.separation < 0.0f || output.separation < 0.0f) {
			output.closestFeatureA.setVertexCount(2);
			output.closestFeatureA.edge[0] = outputA.edgeA1;
			output.closestFeatureA.edge[1] = outputA.edgeA2;

			output.closestFeatureB.setVertexCount(2);
			output.closestFeatureB.edge[0] = output.edgeA1;
			output.closestFeatureB.edge[1] = output.edgeA2;
		}
		else {
			output.closestFeatureA.setVertexCount(1);
			output.closestFeatureA.vertex = output.supportB;

			output.closestFeatureB.setVertexCount(1);
			output.closestFeatureB.vertex = outputA.supportB;
		}

		return output;
	}
}