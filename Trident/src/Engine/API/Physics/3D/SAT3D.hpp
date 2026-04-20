// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	static constexpr uint8 MAX_FACE_VERTEX_COUNT = 8;

	struct EdgeQuery {
		/// The separation between the two bodies.
		float32 separation;

		/// The separating axis.
		float32 axis;

		/// The closest edges on A and B.
		Edge3D edgeA;
		Edge3D edgeB;
	};

	struct FaceQuery {
		/// The separation between the two bodies.
		float32 separation;

		/// The separating axis (the normal of the reference face).
		vec3<float32> axis;

		/// The closest feature (face or edge) on the reference body, facing towards the incident body.
		uint16 referenceFaceIndex;

		const RigidBody3DComponent* referenceBody;
		const RigidBody3DComponent* incidentBody;

		const Hull3D* referenceHull;
		const Hull3D* incidentHull;
	};

	struct SATOutput3D {
		bool overlap;

		EdgeQuery edgeQuery;
		FaceQuery faceQueryA;
		FaceQuery faceQueryB;
	};

	/// The Separating Axis Theorem algorithm in 3D.
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	// TODO(physics/3d/sat-frame-coherence): Cache the separation axis from the last frame to make the algorithm O(1) in non-intersection cases.
	SATOutput3D SAT3D(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, const Hull3D& hullA, const Hull3D& hullB);
}