#pragma once

#include "Engine/API/Physics/Body/Body.hpp"

namespace ProjectA {
	struct RigidBody2D : public Body {
		vec2<float32> position = {0.0f, 0.0f};
		vec2<float32> linearVelocity = {0.0f, 0.0f};
		vec2<float32> linearAcceleration = {0.0f, 0.0f};

		// Sum of the forces acting on the object's linear properties.
		vec2<float32> linearForces = {0.0f, 0.0f};

		float32 rotation = 0.0f;
		float32 angularVelocity = 0.0f;
		float32 angularAcceleration = 0.0f; // (torque)

		// TODO: Angular forces?

		vec2<float32> scale = {1.0f, 1.0f};

		// TODO: Scaling forces?

		float32 inverseMass = 1.0f / 1.0_kg; // (kg)

		// TODO: Inertia/inverse inertia?
	};
}