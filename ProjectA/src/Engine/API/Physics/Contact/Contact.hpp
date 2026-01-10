#pragma once

namespace ProjectA {
	struct Contact2DID {
		static constexpr uint32 NULL_EDGE_INDEX = max<uint32>();

		// The edge is represented in a clockwise order, so the normal is pointing left.
		uint32 referenceEdge0Index = NULL_EDGE_INDEX;
		uint32 referenceEdge1Index = NULL_EDGE_INDEX;

		// The edge is represented in a clockwise order, so the normal is pointing left.
		uint32 incidentEdge0Index = NULL_EDGE_INDEX;
		uint32 incidentEdge1Index = NULL_EDGE_INDEX;

		bool operator ==(const Contact2DID& other) const {
			return (
				referenceEdge0Index == other.referenceEdge0Index &&
				referenceEdge1Index == other.referenceEdge1Index &&
				incidentEdge0Index == other.incidentEdge0Index &&
				incidentEdge1Index == other.incidentEdge1Index
			);
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

		bool operator ==(const Contact2D& other) const {
			return id == other.id;
		}
	};
}