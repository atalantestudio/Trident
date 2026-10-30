// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	struct RigidBodyPair3D {
		const Hull3D* hullA = nullptr;
		const Hull3D* hullB = nullptr;

		RigidBody3DComponent* rigidBodyA = nullptr;
		RigidBody3DComponent* rigidBodyB = nullptr;
	};

	inline bool operator<(const RigidBodyPair3D& a, const RigidBodyPair3D& b) {
		if (a.rigidBodyA < b.rigidBodyA) {
			return true;
		}

		if (a.rigidBodyA == b.rigidBodyA && a.rigidBodyB < b.rigidBodyB) {
			return true;
		}

		return false;
	}
}