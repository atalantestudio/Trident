#pragma once

#include "Engine/API/Geometry/PointGeometry2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"
#include "Engine/API/Physics/SATCircleConvexHull2D.hpp"

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
		const mat2x2<float32> R = mat2x2_rotation(-incidentBody.rotation);

		float32 minDotProduct = max<float32>();
		ClipFeature2D incidentEdge{};
		uint32 incidentEdgeIndex = 0;

		incidentEdge.setVertexCount(2);

		// TODO: Expose edge normals from ConvexHullGeometry.
		for (uint32 edgeIndex = 0; edgeIndex < geometry->edges.count; edgeIndex += 1) {
			const Edge& edge = geometry->edges[edgeIndex];
			const vec2<float32>& a = incidentBody.geometry->vertices[edge.indexA];
			const vec2<float32>& b = incidentBody.geometry->vertices[edge.indexB];

			const vec2<float32> edgeNormal = edgeNormalClockwise(a, b) * R;

			const float32 dotProduct = dot(referenceEdgeNormal, edgeNormal);

			if (dotProduct < minDotProduct) {
				minDotProduct = dotProduct;
				incidentEdge.edge[0].position = a;
				incidentEdge.edge[1].position = b;
				incidentEdgeIndex = edgeIndex;
			}
		}

		incidentEdge.edge[0].position = multiplyTransform(incidentEdge.edge[0].position, incidentBody.transform);
		incidentEdge.edge[0].id.incidentEdge0Index = (incidentEdgeIndex - 1) % geometry->edges.count;
		incidentEdge.edge[0].id.incidentEdge1Index = incidentEdgeIndex;

		incidentEdge.edge[1].position = multiplyTransform(incidentEdge.edge[1].position, incidentBody.transform);
		incidentEdge.edge[1].id.incidentEdge0Index = incidentEdgeIndex;
		incidentEdge.edge[1].id.incidentEdge1Index = (incidentEdgeIndex + 1) % geometry->edges.count;

		return incidentEdge;
	}

	// `normal` and `offset` form the line equation.
	static ClipFeature2D clipEdge(const ClipFeature2D& edge, const vec2<float32>& normal, float32 offset, uint32 edgeIndex) {
		ClipFeature2D clippedEdge;
		uint32 clippedEdgeVertexCount = 0;

		const float32 d0 = dot(edge.edge[0].position, normal) - offset;

		if (d0 <= 0.0f) {
			// This vertex is below the line; don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[0];
			clippedEdgeVertexCount += 1;
		}

		const float32 d1 = dot(edge.edge[1].position, normal) - offset;

		if (d1 <= 0.0f) {
			// This vertex is below the line; don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[1];
			clippedEdgeVertexCount += 1;
		}

		// If one vertex is above and the other is below, interpolate.
		if (d0 * d1 < 0.0f) {
			DEBUG_ASSERT(clippedEdgeVertexCount < 2);

			const float32 t = d0 / (d0 - d1);

			clippedEdge.edge[clippedEdgeVertexCount].position = lerp(edge.edge[0].position, edge.edge[1].position, t);

			if (d0 > 0.0f) {
				clippedEdge.edge[clippedEdgeVertexCount].id.referenceEdge0Index = edgeIndex;
				clippedEdge.edge[clippedEdgeVertexCount].id.incidentEdge0Index = Contact2DID::NULL_EDGE_INDEX;
			}
			else {
				clippedEdge.edge[clippedEdgeVertexCount].id.referenceEdge1Index = edgeIndex;
				clippedEdge.edge[clippedEdgeVertexCount].id.incidentEdge1Index = Contact2DID::NULL_EDGE_INDEX;
			}

			clippedEdgeVertexCount += 1;
		}

		// If both vertices are above the line, clip them.

		DEBUG_ASSERT(clippedEdgeVertexCount <= 2);

		clippedEdge.setVertexCount(clippedEdgeVertexCount);

		return clippedEdge;
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
		const float32 lengthAB = lengthSq(AB);

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
		contactManifold->setContactCount(1);
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
			if (visualizationData != nullptr) {
				visualizationData->gjkOutput = gjkOutput;
			}
		#endif

		const float32 d = gjkOutput.distance - A.geometry->radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -A.geometry->radius * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->setContactCount(1);
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B);

		#ifdef BUILD_MODE_DEBUG
			if (visualizationData != nullptr) {
				visualizationData->satOutput = satOutput;
			}
		#endif

		contactManifold->normal = -satOutput.normal;
		contactManifold->setContactCount(1);
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
			if (visualizationData != nullptr) {
				visualizationData->satOutput = satOutput;
			}
		#endif

		if (satOutput.separation > 0.0f) {
			return false;
		}

		const ClipFeature2D incidentEdge = findIncidentEdge(*satOutput.incidentBody, satOutput.normal);
		const vec2<float32> referenceRightEdgeNormal = normalized(satOutput.referenceEdge.edge[1] - satOutput.referenceEdge.edge[0]);

		const ref<ConvexHullGeometry2D> referenceGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(satOutput.referenceBody->geometry);

		const uint32 leftEdgeIndex = (satOutput.referenceEdgeIndex - 1) % referenceGeometry->edges.count;

		ClipFeature2D clippedIncidentEdge = incidentEdge;
		clippedIncidentEdge = clipEdge(clippedIncidentEdge, -referenceRightEdgeNormal, -dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[0]), leftEdgeIndex);

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		const uint32 rightEdgeIndex = (satOutput.referenceEdgeIndex + 1) % referenceGeometry->edges.count;

		clippedIncidentEdge = clipEdge(clippedIncidentEdge, referenceRightEdgeNormal, dot(referenceRightEdgeNormal, satOutput.referenceEdge.edge[1]), rightEdgeIndex);

		if (clippedIncidentEdge.getVertexCount() < 2) {
			return false;
		}

		contactManifold->normal = satOutput.normal;

		if (satOutput.referenceBody == &B) {
			// The normal must point towards the incident body and away from the reference body.
			contactManifold->normal = -contactManifold->normal;

			// TODO
			/*uint32 temp;

			temp = clippedIncidentEdge.edge[0].id.incidentEdge0Index;
			clippedIncidentEdge.edge[0].id.referenceEdge0Index = temp;

			temp = clippedIncidentEdge.edge[0].id.incidentEdge1Index;
			clippedIncidentEdge.edge[0].id.referenceEdge1Index = temp;

			temp = clippedIncidentEdge.edge[1].id.incidentEdge0Index;
			clippedIncidentEdge.edge[1].id.referenceEdge0Index = temp;

			temp = clippedIncidentEdge.edge[1].id.incidentEdge1Index;
			clippedIncidentEdge.edge[1].id.referenceEdge1Index = temp;*/
		}

		const float32 referenceNormalOffset = dot(satOutput.referenceEdge.edge[0], satOutput.normal);
		uint32 contactCount = 0;

		// Keep all vertices below the reference edge, then move them to the reference edge's line.
		for (uint32 vertexIndex = 0; vertexIndex < 2; vertexIndex += 1) {
			const float32 separation = dot(clippedIncidentEdge.edge[vertexIndex].position, satOutput.normal) - referenceNormalOffset;

			if (separation > 0.0f) {
				continue;
			}

			contactManifold->contacts[contactCount].position = clippedIncidentEdge.edge[vertexIndex].position - satOutput.normal * separation;
			contactManifold->contacts[contactCount].separation = separation;
			contactManifold->contacts[contactCount].id = clippedIncidentEdge.edge[vertexIndex].id;

			contactCount += 1;
		}

		contactManifold->setContactCount(contactCount);

		return contactCount;
	}

	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider    },
		{nullptr,              ConvexHullConvexHullCollider},
	};
}