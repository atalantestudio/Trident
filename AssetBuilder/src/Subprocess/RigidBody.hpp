// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Physics/MomentOfInertia.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	template<>
	struct Descriptor<RigidBody2DComponent> {
		svec2<float32> position;
		float32 rotation;
		svec2<float32> scale;

		svec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;

		/// Uniform mass distribution is assumed.
		float32 mass;

		MomentOfInertia2D momentOfInertia;

		float32 friction;
		float32 restitution;
	};

	template<>
	struct Descriptor<RigidBody3DComponent> {
		svec3<float32> position;
		svec3<float32> rotation;
		float32 uniformScale = 1.0f;
		svec3<float32> linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};
		svec3<float32> angularVelocity = {0.0_w, 0.0_w, 0.0_w};

		/// Uniform mass distribution is assumed.
		float32 mass;

		MomentOfInertia3D momentOfInertia;

		// TODO(physics/3d/uniform-scaling): Deprecate.
		svec3<float32> inertiaScale;

		float32 friction;
		float32 restitution;
	};

	RigidBody2DComponent createRigidBody(const Descriptor<RigidBody2DComponent>& descriptor);
	RigidBody3DComponent createRigidBody(const Descriptor<RigidBody3DComponent>& descriptor);
}