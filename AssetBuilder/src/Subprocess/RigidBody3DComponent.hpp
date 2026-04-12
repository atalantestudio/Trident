// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	template<>
	struct Descriptor<RigidBody3DComponent> {
		svec3<float32> position;
		svec3<float32> rotation;
		svec3<float32> scale;
	};

	inline RigidBody3DComponent createRigidBody(const Descriptor<RigidBody3DComponent>& descriptor) {
		RigidBody3DComponent component = {
			.position = {descriptor.position[0], descriptor.position[1], descriptor.position[2]},
			.rotation = {descriptor.rotation[0], descriptor.rotation[1], descriptor.rotation[2]},
			.scale = {descriptor.scale[0], descriptor.scale[1], descriptor.scale[2]},
		};

		component.updateWorld({0.0_m, 0.0_m, 0.0_m});

		return component;
	}
}