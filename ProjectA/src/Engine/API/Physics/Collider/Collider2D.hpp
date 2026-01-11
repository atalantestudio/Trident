#pragma once

#include "Engine/API/Geometry/PointGeometry2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"
#include "Engine/API/Physics/SATCircleConvexHull2D.hpp"
#include "Engine/API/Physics/clipEdge.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Physics/VisualizationData.hpp"
#endif

namespace ProjectA {
	typedef bool (*Collider2D)(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	);

	// Finds and returns the incident edge feature on a convex hull.
	static ClipFeature2D findIncidentEdge(const RigidBody2D& incidentBody, const vec2<float32>& referenceEdgeNormal) {
		const ref<ConvexHullGeometry2D> geometry = std::static_pointer_cast<ConvexHullGeometry2D>(incidentBody.geometry);
		const mat2x2<float32> R = transpose(mat2x2<float32>::rotation(incidentBody.rotation)); // R == RotT

		float32 minAngle = max<float32>();
		sint32 incidentEdgeIndex = 0;

		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		// TODO: Expose edge normals from ConvexHullGeometry.
		for (uint32 edgeIndex = 0; edgeIndex < geometry->edges.count(); edgeIndex += 1) {
			const Edge& edge = geometry->edges[edgeIndex];
			const vec2<float32> a = incidentBody.geometry->vertices[edge.indexA];
			const vec2<float32> b = incidentBody.geometry->vertices[edge.indexB];
			const vec2<float32> edgeNormal = edgeNormalClockwise(a, b) * R;
			const float32 angle = dot(referenceEdgeNormal, edgeNormal);

			if (angle < minAngle) {
				minAngle = angle;

				// TODO
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;

				incidentEdgeIndex = edgeIndex;
			}
		}

		incidentEdge.edge[0].position = multiplyTransform(incidentEdge.edge[0].position, incidentBody.transform);
		incidentEdge.edge[0].id.incidentEdge0Index = (incidentEdgeIndex + 1) % geometry->edges.count();
		incidentEdge.edge[0].id.incidentEdge1Index = incidentEdgeIndex;

		incidentEdge.edge[1].position = multiplyTransform(incidentEdge.edge[1].position, incidentBody.transform);
		incidentEdge.edge[1].id.incidentEdge0Index = incidentEdgeIndex;
		incidentEdge.edge[1].id.incidentEdge1Index = (incidentEdgeIndex - 1) % geometry->edges.count();

		return incidentEdge;
	}

	static bool CircleCircleCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const vec2<float32> AB = B.position - A.position;
		const float32 lengthAB = lengthSquared(AB);

		if (abs(lengthAB) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		// NOTE: The Y component is ignored for circle scaling.
		const float32 scaleA = A.scale[0];
		const float32 scaleB = B.scale[0];

		const float32 d = lengthAB - pow(A.geometry->radius * scaleA + B.geometry->radius * scaleB, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = A.position + A.geometry->support(AB) * scaleA;
		const vec2<float32> sB = B.position + B.geometry->support(-AB) * scaleB;

		contactManifold->normal = normalized(AB);
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;
		contactManifold->contacts[0].separation = d;

		return true;
	}

	static bool CircleConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		RigidBody2D pointA = A;
		pointA.geometry = createRef<PointGeometry2D>();

		// NOTE: The Y component is ignored for circle scaling.
		const float32 scaleA = A.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(pointA, B);

		#ifdef BUILD_MODE_DEBUG
			visualizationData->gjkOutput = gjkOutput;
		#endif

		const float32 d = gjkOutput.distance - A.geometry->radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -A.geometry->radius * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B);

		#ifdef BUILD_MODE_DEBUG
			visualizationData->satOutput = satOutput;
		#endif

		contactManifold->normal = -satOutput.normal;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.normal * satOutput.separation;
		contactManifold->contacts[0].separation = satOutput.separation - A.geometry->radius * scaleA;

		return true;
	}

	static bool ConvexHullConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const SAT2DOutput satOutput = SAT2D(A, B);

		#ifdef BUILD_MODE_DEBUG
			visualizationData->satOutput = satOutput;
		#endif

		if (satOutput.separation > 0.0f) {
			return false;
		}

		ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, satOutput.normal);

		const vec2<float32> referenceRightEdgeNormal = normalized(satOutput.referenceEdge.edge[1] - satOutput.referenceEdge.edge[0]);
		const ref<ConvexHullGeometry2D> referenceGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(satOutput.referenceBody->geometry);
		const uint32 leftEdgeIndex = (static_cast<sint32>(satOutput.referenceEdgeIndex) - 1) % referenceGeometry->edges.count();

		incidentEdge = clipEdge(incidentEdge, -referenceRightEdgeNormal, -dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[0]), leftEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		const uint32 rightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % referenceGeometry->edges.count();

		incidentEdge = clipEdge(incidentEdge, referenceRightEdgeNormal, dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[1]), rightEdgeIndex);

		if (incidentEdge.vertexCount < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;

		if (satOutput.referenceBody == &B) {
			contactManifold->normal *= -1.0f;
		}

		const float32 referenceNormalOffset = dot(satOutput.referenceEdge.edge[0], satOutput.normal);

		uint32 vertexIndex = 0;
		uint32 contactCount = 0;

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		while (vertexIndex < 2) {
			const ClipVertex2D& clipVertex = incidentEdge.edge[vertexIndex];
			const float32 separation = dot(clipVertex.position, satOutput.normal) - referenceNormalOffset;

			if (separation <= 0.0f) {
				Contact2D& contact = contactManifold->contacts[contactCount];
				contact.position = clipVertex.position - satOutput.normal * separation;
				contact.separation = separation;
				contact.id = clipVertex.id;

				if (satOutput.referenceBody == &B) {
					contact.id.flip();
				}

				contactCount += 1;
			}

			vertexIndex += 1;
		}

		contactManifold->contactCount = contactCount;

		return contactCount;
	}

	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider    },
		{nullptr,              ConvexHullConvexHullCollider},
	};
}