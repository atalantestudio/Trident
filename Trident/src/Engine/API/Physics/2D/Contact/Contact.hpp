// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct ContactID2D {
		static constexpr uint16 NULL_EDGE_INDEX = max<uint16>();

		inline bool operator==(const ContactID2D& other) const {
			return *pointer_cast<const uint64*>(this) == *pointer_cast<const uint64*>(&other);
		}

		inline void flip() {
			std::swap(referenceEdgeAIndex, referenceEdgeBIndex);
			std::swap(incidentEdgeAIndex, incidentEdgeBIndex);
		}

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 referenceEdgeAIndex = NULL_EDGE_INDEX;
		uint16 referenceEdgeBIndex = NULL_EDGE_INDEX;

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 incidentEdgeAIndex = NULL_EDGE_INDEX;
		uint16 incidentEdgeBIndex = NULL_EDGE_INDEX;
	};

	struct Contact2D {
		// Position of the contact in world space.
		vec2<float32> position;

		// Distance to the reference edge. Always negative.
		float32 separation;

		// Vector from A's position to the contact in world space.
		vec2<float32> rA;

		// Vector from B's position to the contact in world space.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		ContactID2D id;

		float32 accumulatedNormalImpulse;
		float32 accumulatedTangentImpulse;
	};
}