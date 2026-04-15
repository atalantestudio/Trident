// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	struct SATOutput3D {
		/// The separation between the two bodies.
		float32 separation;

		/// The separating axis (the normal of the reference feature).
		vec3<float32> axis;

		/// Closest feature on the reference body to the incident body.
		// TODO(physics/3d/feature)
		// Feature3D referenceFeature;
		uint32 referenceFaceIndex;

		/// The closest point on the incident body to the reference body.
		vec3<float32> incidentSupportPoint;

		const RigidBody3DComponent* referenceBody;
		const RigidBody3DComponent* incidentBody;

		const Hull3D* referenceHull;
		const Hull3D* incidentHull;
	};

	/// The Separating Axis Theorem algorithm in 3D.
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	// TODO(physics/3d/sat-frame-coherence): Cache the separation axis from the last frame to make the algorithm O(1) in non-intersection cases.
	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB);
}