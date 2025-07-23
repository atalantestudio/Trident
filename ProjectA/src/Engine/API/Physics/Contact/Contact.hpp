#pragma once

namespace ProjectA {
	struct Contact2DID {
		static constexpr uint32 NULL_EDGE_INDEX = max<uint32>();

		uint32 edgeIndexA0 = NULL_EDGE_INDEX;
		uint32 edgeIndexA1 = NULL_EDGE_INDEX;
		uint32 edgeIndexB0 = NULL_EDGE_INDEX;
		uint32 edgeIndexB1 = NULL_EDGE_INDEX;

		bool operator ==(const Contact2DID& other) const {
			return (
				*pointer_cast<const uint64*>(&edgeIndexA0) == *pointer_cast<const uint64*>(&other.edgeIndexA0) &&
				*pointer_cast<const uint64*>(&edgeIndexB0) == *pointer_cast<const uint64*>(&other.edgeIndexB0)
			);
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