// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Finds and returns the incident edge on the provided rigid body.
	inline ClipFeature2D findIncidentEdge(
		/// The rigid body owning the incident edge.
		const RigidBody2DComponent& incidentBody,

		/// The hull used by the rigid body.
		const Hull2D& incidentHull,

		/// The reference edge normal, rotated by (Reference - Incident).
		vec2<float32> referenceEdgeNormal
	) {
		ClipFeature2D incidentEdge{};
		incidentEdge.vertexCount = 2;

		const uint16 edgeCount = static_cast<uint16>(incidentHull.edges.count());

		float32 angle;
		float32 minAngle = max<float32>();
		uint32 incidentEdgeIndex = 0;
		uint32 edgeIndex = 0;

		while (edgeIndex < edgeCount) {
			const Edge2D edge = incidentHull.edges[edgeIndex];
			const vec2<float32> a = incidentHull.vertices[edge.indexA];
			const vec2<float32> b = incidentHull.vertices[edge.indexB];
			const vec2<float32> d = perpendicularLeft(b - a);

			angle = dot(d, referenceEdgeNormal);

			if (angle < minAngle) {
				minAngle = angle;

				// TODO(physics/2d/incident-edge-vertex-order): Why is the simulation unstable when a and b are inverted?
				incidentEdge.edge[0].position = b;
				incidentEdge.edge[1].position = a;

				incidentEdgeIndex = edgeIndex;
			}

			edgeIndex += 1;
		}

		ClipVertex2D& a = incidentEdge.edge[0];
		a.position = a.position * incidentBody.world;
		a.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		a.id.incidentEdgeAIndex = (incidentEdgeIndex + 1) % edgeCount;
		a.id.incidentEdgeBIndex = incidentEdgeIndex;

		ClipVertex2D& b = incidentEdge.edge[1];
		b.position = b.position * incidentBody.world;
		b.id.referenceEdgeAIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
		b.id.incidentEdgeAIndex = incidentEdgeIndex;
		b.id.incidentEdgeBIndex = (static_cast<sint16>(incidentEdgeIndex) - 1) % edgeCount;

		return incidentEdge;
	}
}