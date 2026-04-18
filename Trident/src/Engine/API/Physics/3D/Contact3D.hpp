// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Contact3D {
		/// Position of the contact in world space.
		vec3<float32> position;

		/// Distance to the edge of the reference body.
		float32 separation;

		/// Vector from A's position to the contact.
		vec3<float32> rA;

		/// Vector from B's position to the contact.
		vec3<float32> rB;

		float32 massNormal;
		float32 massTangent0;
		float32 massTangent1;
		float32 bias;

		// TODO(physics/3d/contact-id)

		float32 accumulatedNormalImpulse;
		float32 accumulatedTangentImpulse;
	};
}