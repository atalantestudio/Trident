// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Physics/MomentOfInertia.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/Filesystem/Asset/Geometry2DAsset.hpp"

namespace Trident {
	template<>
	struct Descriptor<RigidBody2DComponent> {
		/// Required when the moment of inertia is POLYGON.
		ref<Geometry2DAsset> geometry = nullptr;

		vec2<float32> position;
		float32 rotation;
		vec2<float32> scale;

		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;

		/// Uniform mass distribution is assumed.
		float32 mass;

		MomentOfInertia2D momentOfInertia;

		float32 friction;
		float32 restitution;
	};
}