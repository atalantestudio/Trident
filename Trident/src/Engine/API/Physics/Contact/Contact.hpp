// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Contact2DID {
		static constexpr uint16 NULL_EDGE_INDEX = max<uint16>();

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 referenceEdgeAIndex = NULL_EDGE_INDEX;
		uint16 referenceEdgeBIndex = NULL_EDGE_INDEX;

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 incidentEdgeAIndex = NULL_EDGE_INDEX;
		uint16 incidentEdgeBIndex = NULL_EDGE_INDEX;

		inline bool operator==(const Contact2DID& other) const {
			return *pointer_cast<const uint64*>(this) == *pointer_cast<const uint64*>(&other);
		}

		inline void flip() {
			std::swap(referenceEdgeAIndex, referenceEdgeBIndex);
			std::swap(incidentEdgeAIndex, incidentEdgeBIndex);
		}
	};

	struct Contact2D {
		vec2<float32> position;

		// Distance to the reference edge.
		float32 separation;

		// A's position to the contact.
		vec2<float32> rA;

		// B's position to the contact.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		Contact2DID id;

		float32 accumulatedNormalImpulse;
		float32 accumulatedTangentImpulse;
	};
}