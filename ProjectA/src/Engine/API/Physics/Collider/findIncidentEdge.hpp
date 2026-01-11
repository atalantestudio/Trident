// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace ProjectA {
	// Finds and returns the incident edge feature on a convex hull.
	ClipFeature2D findIncidentEdge(const RigidBody2D& incidentBody, vec2<float32> referenceEdgeNormal) {
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
		incidentEdge.edge[0].id.incidentEdge0Index = (incidentEdgeIndex + 1) % static_cast<uint16>(geometry->edges.count());
		incidentEdge.edge[0].id.incidentEdge1Index = incidentEdgeIndex;

		incidentEdge.edge[1].position = multiplyTransform(incidentEdge.edge[1].position, incidentBody.transform);
		incidentEdge.edge[1].id.incidentEdge0Index = incidentEdgeIndex;
		incidentEdge.edge[1].id.incidentEdge1Index = (incidentEdgeIndex - 1) % static_cast<uint16>(geometry->edges.count());

		return incidentEdge;
	}
}