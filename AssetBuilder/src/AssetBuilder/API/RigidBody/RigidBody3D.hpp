// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Physics/MomentOfInertia.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	template<>
	struct Descriptor<RigidBody3DComponent> {
		vec3<float32> position;
		vec3<float32> rotation;
		float32 uniformScale = 1.0f;
		vec3<float32> linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};
		vec3<float32> angularVelocity = {0.0_w, 0.0_w, 0.0_w};

		/// Uniform mass distribution is assumed.
		float32 mass;

		MomentOfInertia3D momentOfInertia;

		// TODO(physics/3d/uniform-scaling): Deprecate.
		vec3<float32> inertiaScale;

		float32 friction;
		float32 restitution;
	};
}