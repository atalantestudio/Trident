#pragma once

namespace Trident {
	struct Contact2DID {
		static constexpr uint16 NULL_EDGE_INDEX = max<uint16>();

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 referenceEdge0Index = NULL_EDGE_INDEX;
		uint16 referenceEdge1Index = NULL_EDGE_INDEX;

		// The edge is represented in a clockwise order, so the normal is pointing left (outside of the body).
		uint16 incidentEdge0Index = NULL_EDGE_INDEX;
		uint16 incidentEdge1Index = NULL_EDGE_INDEX;

		bool operator ==(const Contact2DID& other) const {
			return *pointer_cast<const uint64*>(this) == *pointer_cast<const uint64*>(&other);
		}

		void flip() {
			std::swap(referenceEdge0Index, referenceEdge1Index);
			std::swap(incidentEdge0Index, incidentEdge1Index);
		}
	};

	struct Contact2D {
		vec2<float32> position;

		// Distance to the normal of the reference edge.
		float32 separation;

		// A's position to the contact.
		vec2<float32> rA;

		// B's position to the contact.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		Contact2DID id;

		float32 accumulatedNormalImpulse = 0.0f;
		float32 accumulatedTangentImpulse = 0.0f;
	};
}