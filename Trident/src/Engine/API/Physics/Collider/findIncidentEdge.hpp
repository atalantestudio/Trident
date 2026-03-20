// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Finds and returns the incident edge on the provided rigid body.
	inline ClipFeature2D findIncidentEdge(
		/// The rigid body owning the incident edge.
		const RigidBody2D& incidentBody,

		/// The reference edge normal, rotated by (Reference - Incident).
		vec2<float32> referenceEdgeNormal
	) {
		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		const ref<ConvexHullGeometry2D> incidentGeometry = std::static_pointer_cast<ConvexHullGeometry2D>(incidentBody.geometry);

		float32 minAngle = max<float32>();
		uint32 incidentEdgeIndex = 0;
		uint32 edgeIndex = 0;

		// TODO: Expose edge normals from ConvexHullGeometry.
		while (edgeIndex < incidentGeometry->edges.count()) {
			const Edge edge = incidentGeometry->edges[edgeIndex];
			const vec2<float32> a = incidentGeometry->vertices[edge.indexA];
			const vec2<float32> b = incidentGeometry->vertices[edge.indexB];
			const vec2<float32> d = perpendicularLeft(b - a);
			const float32 angle = dot(referenceEdgeNormal, d);

			if (angle < minAngle) {
				minAngle = angle;
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;
				incidentEdgeIndex = edgeIndex;
			}

			edgeIndex += 1;
		}

		incidentEdge.edge[0].position = multiplyTransform(incidentEdge.edge[0].position, incidentBody.transform);
		incidentEdge.edge[0].id.incidentEdge0Index = (incidentEdgeIndex + 1) % static_cast<uint16>(incidentGeometry->edges.count());
		incidentEdge.edge[0].id.incidentEdge1Index = incidentEdgeIndex;

		incidentEdge.edge[1].position = multiplyTransform(incidentEdge.edge[1].position, incidentBody.transform);
		incidentEdge.edge[1].id.incidentEdge0Index = incidentEdgeIndex;
		incidentEdge.edge[1].id.incidentEdge1Index = static_cast<sint32>(incidentEdgeIndex - 1) % static_cast<uint16>(incidentGeometry->edges.count());

		return incidentEdge;
	}
}