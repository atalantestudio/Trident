// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Finds and returns the incident edge on the provided rigid body.
	inline ClipFeature2D findIncidentEdge(
		/// The rigid body owning the incident edge.
		const RigidBody2D& incidentBody,

		/// The hull used by the rigid body.
		const Hull2D& incidentHull,

		/// The reference edge normal, rotated by (Reference - Incident).
		vec2<float32> referenceEdgeNormal
	) {
		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		float32 minAngle = max<float32>();
		uint32 incidentEdgeIndex = 0;
		uint32 edgeIndex = 0;

		// TODO: Expose edge normals from ConvexHullGeometry.
		while (edgeIndex < incidentHull.getEdges().count()) {
			const Edge edge = incidentHull.getEdges()[edgeIndex];
			const vec2<float32> a = incidentHull.getVertices()[edge.indexA];
			const vec2<float32> b = incidentHull.getVertices()[edge.indexB];
			const vec2<float32> d = perpendicularLeft(b - a);
			const float32 angle = dot(referenceEdgeNormal, d);

			if (angle < minAngle) {
				minAngle = angle;

				// TODO: Why a and b need to be inverted?
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;

				incidentEdgeIndex = edgeIndex;
			}

			edgeIndex += 1;
		}

		incidentEdge.edge[0].position = multiplyTransform(incidentEdge.edge[0].position, incidentBody.world);
		incidentEdge.edge[0].id.incidentEdge0Index = (incidentEdgeIndex + 1) % static_cast<uint16>(incidentHull.getEdges().count());
		incidentEdge.edge[0].id.incidentEdge1Index = incidentEdgeIndex;

		incidentEdge.edge[1].position = multiplyTransform(incidentEdge.edge[1].position, incidentBody.world);
		incidentEdge.edge[1].id.incidentEdge0Index = incidentEdgeIndex;
		incidentEdge.edge[1].id.incidentEdge1Index = static_cast<sint32>(incidentEdgeIndex - 1) % static_cast<uint16>(incidentHull.getEdges().count());

		return incidentEdge;
	}
}