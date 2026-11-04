// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	struct EdgeQuery {
		/// The separation between the two bodies.
		float32 separation = min<float32>();

		/// The indices of the intersecting edges.
		uint16 edgeIndexA = max<uint16>();
		uint16 edgeIndexB = max<uint16>();
	};

	struct FaceQuery {
		/// The separation between the two bodies.
		float32 separation;

		/// The closest feature (face or edge) on the reference body, facing towards the incident body.
		uint16 referenceFaceIndex;

		/// The reference plane normal is the separating axis.
		Plane3D referenceFace;

		Transform3D transform;
	};

	struct SATOutput3D {
		bool overlap;

		FaceQuery faceQueryA;
		FaceQuery faceQueryB;
		EdgeQuery edgeQuery;
	};

	// TODO(physics/3d/sat/frame-coherence): Cache the separation axis from the last frame to make the algorithm O(1) in non-intersection cases.
	/// A Separating Axis Theorem implementation in 3D.
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB);
}